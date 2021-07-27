package awais.instagrabber.adapters.viewholder.directmessages;

import android.content.res.Resources;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;

import com.facebook.drawee.generic.GenericDraweeHierarchyBuilder;
import com.facebook.drawee.generic.RoundingParams;
import com.facebook.drawee.view.SimpleDraweeView;
import com.google.common.collect.ImmutableList;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmProfileBinding;
import awais.instagrabber.models.enums.DirectItemType;
import awais.instagrabber.repositories.responses.Location;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.TextUtils;

public class DirectItemProfileViewHolder extends DirectItemViewHolder {

    private final LayoutDmProfileBinding binding;
    private final ImmutableList<SimpleDraweeView> previewViews;

    public DirectItemProfileViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                       @NonNull final LayoutDmProfileBinding binding,
                                       final User currentUser,
                                       final DirectThread thread,
                                       final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        setItemView(binding.getRoot());
        previewViews = ImmutableList.of(
                binding.preview1,
                binding.preview2,
                binding.preview3,
                binding.preview4,
                binding.preview5,
                binding.preview6
        );
        final Resources resources = itemView.getResources();
        binding.preview4.setHierarchy(new GenericDraweeHierarchyBuilder(resources)
                                              .setRoundingParams(RoundingParams.fromCornersRadii(0, 0, 0, dmRadius))
                                              .build());
        binding.preview6.setHierarchy(new GenericDraweeHierarchyBuilder(resources)
                                              .setRoundingParams(RoundingParams.fromCornersRadii(0, 0, dmRadius, 0))
                                              .build());
    }

    @Override
    public void bindItem(@NonNull final DirectItem item,
                         final MessageDirection messageDirection) {
        binding.getRoot().setBackgroundResource(messageDirection == MessageDirection.INCOMING
                                                ? R.drawable.bg_speech_bubble_incoming
                                                : R.drawable.bg_speech_bubble_outgoing);
        if (item.getItemType() == DirectItemType.PROFILE) {
            setProfile(item);
        } else if (item.getItemType() == DirectItemType.LOCATION) {
            setLocation(item);
        } else {
            return;
        }
        for (final SimpleDraweeView previewView : previewViews) {
            previewView.setImageURI((String) null);
        }
        final List<Media> previewMedias = item.getPreviewMedias();
        if (previewMedias.size() <= 0) {
            binding.firstRow.setVisibility(View.GONE);
            binding.secondRow.setVisibility(View.GONE);
            return;
        }
        if (previewMedias.size() <= 3) {
            binding.firstRow.setVisibility(View.VISIBLE);
            binding.secondRow.setVisibility(View.GONE);
        }
        for (int i = 0; i < previewMedias.size(); i++) {
            final Media previewMedia = previewMedias.get(i);
            if (previewMedia == null) continue;
            final String url = ResponseBodyUtils.getThumbUrl(previewMedia);
            if (url == null) continue;
            previewViews.get(i).setImageURI(url);
        }
    }

    private void setProfile(@NonNull final DirectItem item) {
        final User profile = item.getProfile();
        if (profile == null) return;
        binding.profilePic.setImageURI(profile.getProfilePicUrl());
        binding.username.setText(profile.getUsername());
        final String fullName = profile.getFullName();
        if (!TextUtils.isEmpty(fullName)) {
            binding.fullName.setVisibility(View.VISIBLE);
            binding.fullName.setText(fullName);
        } else {
            binding.fullName.setVisibility(View.GONE);
        }
        binding.isVerified.setVisibility(profile.isVerified() ? View.VISIBLE : View.GONE);
        itemView.setOnClickListener(v -> openProfile(profile.getUsername()));
    }

    private void setLocation(@NonNull final DirectItem item) {
        final Location location = item.getLocation();
        if (location == null) return;
        binding.profilePic.setVisibility(View.GONE);
        binding.username.setText(location.getName());
        final String address = location.getAddress();
        if (!TextUtils.isEmpty(address)) {
            binding.fullName.setText(address);
            binding.fullName.setVisibility(View.VISIBLE);
        } else {
            binding.fullName.setVisibility(View.GONE);
        }
        binding.isVerified.setVisibility(View.GONE);
        itemView.setOnClickListener(v -> openLocation(location.getPk()));
    }

    @Override
    public int getSwipeDirection() {
        return ItemTouchHelper.ACTION_STATE_IDLE;
    }
}
