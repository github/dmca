package awais.instagrabber.adapters.viewholder.directmessages;

import android.text.TextUtils;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.ItemTouchHelper;

import com.facebook.drawee.drawable.ScalingUtils;
import com.facebook.drawee.generic.GenericDraweeHierarchyBuilder;
import com.facebook.drawee.generic.RoundingParams;
import com.google.common.collect.ImmutableList;

import java.util.List;
import java.util.Objects;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.customviews.DirectItemContextMenu;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmMediaShareBinding;
import awais.instagrabber.models.enums.DirectItemType;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Caption;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemClip;
import awais.instagrabber.repositories.responses.directmessages.DirectItemFelixShare;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.NullSafePair;
import awais.instagrabber.utils.NumberUtils;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.Utils;

public class DirectItemMediaShareViewHolder extends DirectItemViewHolder {
    private static final String TAG = DirectItemMediaShareViewHolder.class.getSimpleName();

    private final LayoutDmMediaShareBinding binding;
    private final RoundingParams incomingRoundingParams;
    private final RoundingParams outgoingRoundingParams;
    private DirectItemType itemType;
    private Caption caption;

    public DirectItemMediaShareViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                          @NonNull final LayoutDmMediaShareBinding binding,
                                          final User currentUser,
                                          final DirectThread thread,
                                          final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        incomingRoundingParams = RoundingParams.fromCornersRadii(dmRadiusSmall, dmRadius, dmRadius, dmRadius);
        outgoingRoundingParams = RoundingParams.fromCornersRadii(dmRadius, dmRadiusSmall, dmRadius, dmRadius);
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem item, final MessageDirection messageDirection) {
        binding.topBg.setBackgroundResource(messageDirection == MessageDirection.INCOMING
                                            ? R.drawable.bg_media_share_top_incoming
                                            : R.drawable.bg_media_share_top_outgoing);
        Media media = getMedia(item);
        if (media == null) return;
        itemView.post(() -> {
            setupUser(media);
            setupTitle(media);
            setupCaption(media);
        });
        itemView.post(() -> {
            final MediaItemType mediaType = media.getMediaType();
            setupTypeIndicator(mediaType);
            if (mediaType == MediaItemType.MEDIA_TYPE_SLIDER) {
                setupPreview(media.getCarouselMedia().get(0), messageDirection);
                return;
            }
            setupPreview(media, messageDirection);
        });
        itemView.setOnClickListener(v -> openMedia(media));
    }

    private void setupTypeIndicator(final MediaItemType mediaType) {
        final boolean showTypeIcon = mediaType == MediaItemType.MEDIA_TYPE_VIDEO || mediaType == MediaItemType.MEDIA_TYPE_SLIDER;
        if (!showTypeIcon) {
            binding.typeIcon.setVisibility(View.GONE);
        } else {
            binding.typeIcon.setVisibility(View.VISIBLE);
            binding.typeIcon.setImageResource(mediaType == MediaItemType.MEDIA_TYPE_VIDEO
                                              ? R.drawable.ic_video_24
                                              : R.drawable.ic_checkbox_multiple_blank_stroke);
        }
    }

    private void setupPreview(@NonNull final Media media,
                              final MessageDirection messageDirection) {
        final String url = ResponseBodyUtils.getThumbUrl(media);
        if (Objects.equals(url, binding.mediaPreview.getTag())) {
            return;
        }
        final RoundingParams roundingParams = messageDirection == MessageDirection.INCOMING ? incomingRoundingParams : outgoingRoundingParams;
        binding.mediaPreview.setHierarchy(new GenericDraweeHierarchyBuilder(itemView.getResources())
                                                  .setActualImageScaleType(ScalingUtils.ScaleType.CENTER_CROP)
                                                  .setRoundingParams(roundingParams)
                                                  .build());
        final NullSafePair<Integer, Integer> widthHeight = NumberUtils.calculateWidthHeight(
                media.getOriginalHeight(),
                media.getOriginalWidth(),
                mediaImageMaxHeight,
                mediaImageMaxWidth
        );
        final ViewGroup.LayoutParams layoutParams = binding.mediaPreview.getLayoutParams();
        layoutParams.width = widthHeight.first;
        layoutParams.height = widthHeight.second;
        binding.mediaPreview.requestLayout();
        binding.mediaPreview.setTag(url);
        binding.mediaPreview.setImageURI(url);
    }

    private void setupCaption(@NonNull final Media media) {
        caption = media.getCaption();
        if (caption != null) {
            binding.caption.setVisibility(View.VISIBLE);
            binding.caption.setText(caption.getText());
            binding.caption.setEllipsize(TextUtils.TruncateAt.END);
            binding.caption.setMaxLines(2);
        } else {
            binding.caption.setVisibility(View.GONE);
        }
    }

    private void setupTitle(@NonNull final Media media) {
        final String title = media.getTitle();
        if (!TextUtils.isEmpty(title)) {
            binding.title.setVisibility(View.VISIBLE);
            binding.title.setText(title);
        } else {
            binding.title.setVisibility(View.GONE);
        }
    }

    private void setupUser(@NonNull final Media media) {
        final User user = media.getUser();
        if (user != null) {
            binding.username.setVisibility(View.VISIBLE);
            binding.profilePic.setVisibility(View.VISIBLE);
            binding.username.setText(user.getUsername());
            binding.profilePic.setImageURI(user.getProfilePicUrl());
        } else {
            binding.username.setVisibility(View.GONE);
            binding.profilePic.setVisibility(View.GONE);
        }
    }

    @Nullable
    private Media getMedia(@NonNull final DirectItem item) {
        Media media = null;
        itemType = item.getItemType();
        if (itemType == DirectItemType.MEDIA_SHARE) {
            media = item.getMediaShare();
        } else if (itemType == DirectItemType.CLIP) {
            final DirectItemClip clip = item.getClip();
            if (clip == null) return null;
            media = clip.getClip();
        } else if (itemType == DirectItemType.FELIX_SHARE) {
            final DirectItemFelixShare felixShare = item.getFelixShare();
            if (felixShare == null) return null;
            media = felixShare.getVideo();
        }
        return media;
    }

    @Override
    protected int getReactionsTranslationY() {
        return reactionTranslationYType2;
    }

    @Override
    public int getSwipeDirection() {
        if (itemType != null && (itemType == DirectItemType.CLIP || itemType == DirectItemType.FELIX_SHARE)) {
            return ItemTouchHelper.ACTION_STATE_IDLE;
        }
        return super.getSwipeDirection();
    }

    @Override
    protected List<DirectItemContextMenu.MenuItem> getLongClickOptions() {
        final ImmutableList.Builder<DirectItemContextMenu.MenuItem> builder = ImmutableList.builder();
        if (caption != null && !TextUtils.isEmpty(caption.getText())) {
            builder.add(new DirectItemContextMenu.MenuItem(R.id.copy, R.string.copy_caption, item -> {
                Utils.copyText(itemView.getContext(), caption.getText());
                return null;
            }));
        }
        return builder.build();
    }
}
