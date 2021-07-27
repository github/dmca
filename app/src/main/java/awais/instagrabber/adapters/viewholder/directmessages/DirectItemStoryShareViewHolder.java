package awais.instagrabber.adapters.viewholder.directmessages;

import android.content.res.Resources;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;

import com.facebook.drawee.drawable.ScalingUtils;
import com.facebook.drawee.generic.GenericDraweeHierarchyBuilder;
import com.facebook.drawee.generic.RoundingParams;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmStoryShareBinding;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemStoryShare;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.NullSafePair;
import awais.instagrabber.utils.NumberUtils;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.TextUtils;

public class DirectItemStoryShareViewHolder extends DirectItemViewHolder {

    private final LayoutDmStoryShareBinding binding;

    public DirectItemStoryShareViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                          @NonNull final LayoutDmStoryShareBinding binding,
                                          final User currentUser,
                                          final DirectThread thread,
                                          final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem item, final MessageDirection messageDirection) {
        final Resources resources = itemView.getResources();
        int format = R.string.story_share;
        final String reelType = item.getStoryShare().getReelType();
        if (reelType == null || item.getStoryShare().getMedia() == null) {
            setExpiredStoryInfo(item);
            return;
        }
        if (reelType.equals("highlight_reel")) {
            format = R.string.story_share_highlight;
        }
        final User user = item.getStoryShare().getMedia().getUser();
        final String info = resources.getString(format, user != null ? user.getUsername() : "");
        binding.shareInfo.setText(info);
        binding.text.setVisibility(View.GONE);
        binding.ivMediaPreview.setController(null);
        final DirectItemStoryShare storyShare = item.getStoryShare();
        if (storyShare == null) return;
        setText(storyShare);
        final Media media = storyShare.getMedia();
        setupPreview(messageDirection, media);
        itemView.setOnClickListener(v -> openStory(storyShare));
    }

    private void setupPreview(final MessageDirection messageDirection, final Media storyShareMedia) {
        final MediaItemType mediaType = storyShareMedia.getMediaType();
        binding.typeIcon.setVisibility(mediaType == MediaItemType.MEDIA_TYPE_VIDEO ? View.VISIBLE : View.GONE);
        final RoundingParams roundingParams = messageDirection == MessageDirection.INCOMING
                                              ? RoundingParams.fromCornersRadii(dmRadiusSmall, dmRadius, dmRadius, dmRadius)
                                              : RoundingParams.fromCornersRadii(dmRadius, dmRadiusSmall, dmRadius, dmRadius);
        binding.ivMediaPreview.setHierarchy(new GenericDraweeHierarchyBuilder(itemView.getResources())
                                                    .setRoundingParams(roundingParams)
                                                    .setActualImageScaleType(ScalingUtils.ScaleType.CENTER_CROP)
                                                    .build());
        final NullSafePair<Integer, Integer> widthHeight = NumberUtils.calculateWidthHeight(
                storyShareMedia.getOriginalHeight(),
                storyShareMedia.getOriginalWidth(),
                mediaImageMaxHeight,
                mediaImageMaxWidth
        );
        final ViewGroup.LayoutParams layoutParams = binding.ivMediaPreview.getLayoutParams();
        layoutParams.width = widthHeight.first;
        layoutParams.height = widthHeight.second;
        binding.ivMediaPreview.requestLayout();
        final String thumbUrl = ResponseBodyUtils.getThumbUrl(storyShareMedia);
        binding.ivMediaPreview.setImageURI(thumbUrl);
    }

    private void setText(final DirectItemStoryShare storyShare) {
        final String text = storyShare.getText();
        if (!TextUtils.isEmpty(text)) {
            binding.text.setText(text);
            binding.text.setVisibility(View.VISIBLE);
            return;
        }
        binding.text.setVisibility(View.GONE);
    }

    private void setExpiredStoryInfo(final DirectItem item) {
        binding.shareInfo.setText(item.getStoryShare().getTitle());
        binding.text.setVisibility(View.VISIBLE);
        binding.text.setText(item.getStoryShare().getMessage());
        binding.ivMediaPreview.setVisibility(View.GONE);
        binding.typeIcon.setVisibility(View.GONE);
    }

    @Override
    protected boolean canForward() {
        return false;
    }

    @Override
    public int getSwipeDirection() {
        return ItemTouchHelper.ACTION_STATE_IDLE;
    }
}
