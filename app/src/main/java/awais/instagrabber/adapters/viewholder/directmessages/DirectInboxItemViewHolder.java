package awais.instagrabber.adapters.viewholder.directmessages;

import android.content.res.Resources;
import android.graphics.Typeface;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;
import androidx.constraintlayout.widget.ConstraintSet;
import androidx.recyclerview.widget.RecyclerView;

import com.facebook.drawee.view.SimpleDraweeView;
import com.google.common.collect.ImmutableList;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectMessageInboxAdapter.OnItemClickListener;
import awais.instagrabber.databinding.LayoutDmInboxItemBinding;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.DirectThreadDirectStory;
import awais.instagrabber.utils.DMUtils;
import awais.instagrabber.utils.TextUtils;

public final class DirectInboxItemViewHolder extends RecyclerView.ViewHolder {
    // private static final String TAG = "DMInboxItemVH";
    private final LayoutDmInboxItemBinding binding;
    private final OnItemClickListener onClickListener;
    private final List<SimpleDraweeView> multipleProfilePics;
    private final int childSmallSize;
    private final int childTinySize;

    public DirectInboxItemViewHolder(@NonNull final LayoutDmInboxItemBinding binding,
                                     final OnItemClickListener onClickListener) {
        super(binding.getRoot());
        this.binding = binding;
        this.onClickListener = onClickListener;
        multipleProfilePics = ImmutableList.of(
                binding.multiPic1,
                binding.multiPic2,
                binding.multiPic3
        );
        childSmallSize = itemView.getResources().getDimensionPixelSize(R.dimen.dm_inbox_avatar_size_small);
        childTinySize = itemView.getResources().getDimensionPixelSize(R.dimen.dm_inbox_avatar_size_tiny);
    }

    public void bind(final DirectThread thread) {
        if (thread == null) return;
        if (onClickListener != null) {
            itemView.setOnClickListener((v) -> onClickListener.onItemClick(thread));
        }
        setProfilePics(thread);
        setTitle(thread);
        final List<DirectItem> items = thread.getItems();
        if (items == null || items.isEmpty()) return;
        final DirectItem item = thread.getFirstDirectItem();
        if (item == null) return;
        setDateTime(item);
        setSubtitle(thread);
        setReadState(thread);
    }

    private void setProfilePics(@NonNull final DirectThread thread) {
        final List<User> users = thread.getUsers();
        if (users.size() > 1) {
            binding.profilePic.setVisibility(View.GONE);
            binding.multiPicContainer.setVisibility(View.VISIBLE);
            for (int i = 0; i < Math.min(3, users.size()); ++i) {
                final User user = users.get(i);
                final SimpleDraweeView view = multipleProfilePics.get(i);
                view.setVisibility(user == null ? View.GONE : View.VISIBLE);
                if (user == null) return;
                final String profilePicUrl = user.getProfilePicUrl();
                view.setImageURI(profilePicUrl);
                setChildSize(view, users.size());
                if (i == 1) {
                    updateConstraints(view, users.size());
                }
                view.requestLayout();
            }
            return;
        }
        binding.profilePic.setVisibility(View.VISIBLE);
        binding.multiPicContainer.setVisibility(View.GONE);
        final String profilePicUrl = users.size() == 1 ? users.get(0).getProfilePicUrl() : null;
        if (profilePicUrl == null) {
            binding.profilePic.setController(null);
            return;
        }
        binding.profilePic.setImageURI(profilePicUrl);
    }

    private void updateConstraints(final SimpleDraweeView view, final int length) {
        final ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) view.getLayoutParams();
        if (length >= 2) {
            layoutParams.endToEnd = ConstraintSet.PARENT_ID;
            layoutParams.bottomToBottom = ConstraintSet.PARENT_ID;
        }
        if (length == 3) {
            layoutParams.startToStart = ConstraintSet.PARENT_ID;
            layoutParams.topToTop = ConstraintSet.PARENT_ID;
        }
    }

    private void setChildSize(final SimpleDraweeView view, final int length) {
        final int size = length == 3 ? childTinySize : childSmallSize;
        final ConstraintLayout.LayoutParams viewLayoutParams = new ConstraintLayout.LayoutParams(size, size);
        view.setLayoutParams(viewLayoutParams);
    }

    private void setTitle(@NonNull final DirectThread thread) {
        final String threadTitle = thread.getThreadTitle();
        binding.threadTitle.setText(threadTitle);
    }

    private void setSubtitle(@NonNull final DirectThread thread) {
        final Resources resources = itemView.getResources();
        final long viewerId = thread.getViewerId();
//        final DirectThreadDirectStory directStory = thread.getDirectStory();
//        if (directStory != null && !directStory.getItems().isEmpty()) {
//            final DirectItem item = directStory.getItems().get(0);
//            final MediaItemType mediaType = item.getVisualMedia().getMedia().getMediaType();
//            final String username = DMUtils.getUsername(thread.getUsers(), item.getUserId(), viewerId, resources);
//            final String subtitle = DMUtils.getMediaSpecificSubtitle(username, resources, mediaType);
//            binding.subtitle.setText(subtitle);
//            return;
//        }
        final DirectItem item = thread.getFirstDirectItem();
        if (item == null) return;
        final String subtitle = DMUtils.getMessageString(thread, resources, viewerId, item);
        binding.subtitle.setText(subtitle != null ? subtitle : "");
    }

    private void setDateTime(@NonNull final DirectItem item) {
        final long timestamp = item.getTimestamp() / 1000;
        final String dateTimeString = TextUtils.getRelativeDateTimeString(itemView.getContext(), timestamp);
        binding.tvDate.setText(dateTimeString);
    }

    private void setReadState(@NonNull final DirectThread thread) {
        final boolean read = DMUtils.isRead(thread);
        binding.unread.setVisibility(read ? View.GONE : View.VISIBLE);
        binding.threadTitle.setTypeface(null, read ? Typeface.NORMAL : Typeface.BOLD);
        binding.subtitle.setTypeface(null, read ? Typeface.NORMAL : Typeface.BOLD);
    }
}