package awais.instagrabber.adapters.viewholder;

import android.content.res.ColorStateList;
import android.net.Uri;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.DimenRes;
import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import com.facebook.drawee.backends.pipeline.Fresco;
import com.facebook.drawee.backends.pipeline.PipelineDraweeControllerBuilder;
import com.facebook.imagepipeline.common.ResizeOptions;
import com.facebook.imagepipeline.request.ImageRequest;
import com.facebook.imagepipeline.request.ImageRequestBuilder;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.FeedAdapterV2;
import awais.instagrabber.databinding.ItemFeedGridBinding;
import awais.instagrabber.models.PostsLayoutPreferences;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.DownloadUtils;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.TextUtils;

import static awais.instagrabber.models.PostsLayoutPreferences.PostsLayoutType.STAGGERED_GRID;

public class FeedGridItemViewHolder extends RecyclerView.ViewHolder {
    private final ItemFeedGridBinding binding;

    public FeedGridItemViewHolder(@NonNull final ItemFeedGridBinding binding) {
        super(binding.getRoot());
        this.binding = binding;
    }

    public void bind(final int position,
                     @NonNull final Media media,
                     @NonNull final PostsLayoutPreferences layoutPreferences,
                     final FeedAdapterV2.FeedItemCallback feedItemCallback,
                     final FeedAdapterV2.AdapterSelectionCallback adapterSelectionCallback,
                     final boolean selectionModeActive,
                     final boolean selected) {
        itemView.setOnClickListener(v -> {
            if (!selectionModeActive && feedItemCallback != null) {
                feedItemCallback.onPostClick(media, binding.profilePic, binding.postImage);
                return;
            }
            if (selectionModeActive && adapterSelectionCallback != null) {
                adapterSelectionCallback.onPostClick(position, media);
            }
        });
        if (adapterSelectionCallback != null) {
            itemView.setOnLongClickListener(v -> adapterSelectionCallback.onPostLongClick(position, media));
        }
        binding.selectedView.setVisibility(selected ? View.VISIBLE : View.GONE);
        // for rounded borders (clip view to background shape)
        itemView.setClipToOutline(layoutPreferences.getHasRoundedCorners());
        if (layoutPreferences.getType() == STAGGERED_GRID) {
            final float aspectRatio = (float) media.getOriginalWidth() / media.getOriginalHeight();
            binding.postImage.setAspectRatio(aspectRatio);
        } else {
            binding.postImage.setAspectRatio(1);
        }
        setUserDetails(media, layoutPreferences);
        String thumbnailUrl = null;
        final int typeIconRes;
        switch (media.getMediaType()) {
            case MEDIA_TYPE_IMAGE:
                typeIconRes = -1;
                thumbnailUrl = ResponseBodyUtils.getThumbUrl(media);
                break;
            case MEDIA_TYPE_VIDEO:
                thumbnailUrl = ResponseBodyUtils.getThumbUrl(media);
                typeIconRes = R.drawable.exo_icon_play;
                break;
            case MEDIA_TYPE_SLIDER:
                final List<Media> sliderItems = media.getCarouselMedia();
                if (sliderItems != null) {
                    final Media child = sliderItems.get(0);
                    if (child != null) {
                        thumbnailUrl = ResponseBodyUtils.getThumbUrl(child);
                        if (layoutPreferences.getType() == STAGGERED_GRID) {
                            final float childAspectRatio = (float) child.getOriginalWidth() / child.getOriginalHeight();
                            binding.postImage.setAspectRatio(childAspectRatio);
                        }
                    }
                }
                typeIconRes = R.drawable.ic_checkbox_multiple_blank_stroke;
                break;
            default:
                typeIconRes = -1;
                thumbnailUrl = null;
        }
        setThumbImage(thumbnailUrl);
        if (typeIconRes <= 0) {
            binding.typeIcon.setVisibility(View.GONE);
        } else {
            binding.typeIcon.setVisibility(View.VISIBLE);
            binding.typeIcon.setImageResource(typeIconRes);
        }
        final List<Boolean> checkList = DownloadUtils.checkDownloaded(media);
        if (checkList == null || checkList.isEmpty()) {
            return;
        }
        switch (media.getMediaType()) {
            case MEDIA_TYPE_IMAGE:
            case MEDIA_TYPE_VIDEO:
                binding.downloaded.setVisibility(checkList.get(0) ? View.VISIBLE : View.GONE);
                binding.downloaded.setImageTintList(ColorStateList.valueOf(itemView.getResources().getColor(R.color.green_A400)));
                break;
            case MEDIA_TYPE_SLIDER:
                binding.downloaded.setVisibility(checkList.get(0) ? View.VISIBLE : View.GONE);
                final List<Media> carouselMedia = media.getCarouselMedia();
                boolean allDownloaded = checkList.size() == (carouselMedia == null ? 0 : carouselMedia.size());
                if (allDownloaded) {
                    allDownloaded = checkList.stream().allMatch(downloaded -> downloaded);
                }
                binding.downloaded.setImageTintList(ColorStateList.valueOf(itemView.getResources().getColor(
                        allDownloaded ? R.color.green_A400 : R.color.yellow_400)));
                break;
            default:
        }
    }

    private void setThumbImage(final String thumbnailUrl) {
        if (TextUtils.isEmpty(thumbnailUrl)) {
            binding.postImage.setController(null);
            return;
        }
        final ImageRequest requestBuilder = ImageRequestBuilder.newBuilderWithSource(Uri.parse(thumbnailUrl))
                                                               .setResizeOptions(ResizeOptions.forDimensions(binding.postImage.getWidth(),
                                                                                                             binding.postImage.getHeight()))
                                                               .setLocalThumbnailPreviewsEnabled(true)
                                                               .setProgressiveRenderingEnabled(true)
                                                               .build();
        final PipelineDraweeControllerBuilder builder = Fresco.newDraweeControllerBuilder()
                                                              .setImageRequest(requestBuilder)
                                                              .setOldController(binding.postImage.getController());
        binding.postImage.setController(builder.build());
    }

    private void setUserDetails(@NonNull final Media media,
                                @NonNull final PostsLayoutPreferences layoutPreferences) {
        final User user = media.getUser();
        if (layoutPreferences.isAvatarVisible()) {
            if (user == null) {
                binding.profilePic.setVisibility(View.GONE);
            } else {
                final String profilePicUrl = user.getProfilePicUrl();
                if (TextUtils.isEmpty(profilePicUrl)) {
                    binding.profilePic.setVisibility(View.GONE);
                } else {
                    binding.profilePic.setVisibility(View.VISIBLE);
                    binding.profilePic.setImageURI(profilePicUrl);
                }
            }
            final ViewGroup.LayoutParams layoutParams = binding.profilePic.getLayoutParams();
            @DimenRes final int dimenRes;
            switch (layoutPreferences.getProfilePicSize()) {
                case SMALL:
                    dimenRes = R.dimen.profile_pic_size_small;
                    break;
                case TINY:
                    dimenRes = R.dimen.profile_pic_size_tiny;
                    break;
                default:
                case REGULAR:
                    dimenRes = R.dimen.profile_pic_size_regular;
                    break;
            }
            final int dimensionPixelSize = itemView.getResources().getDimensionPixelSize(dimenRes);
            layoutParams.width = dimensionPixelSize;
            layoutParams.height = dimensionPixelSize;
            binding.profilePic.requestLayout();
        } else {
            binding.profilePic.setVisibility(View.GONE);
        }
        if (layoutPreferences.isNameVisible()) {
            if (user == null) {
                binding.name.setVisibility(View.GONE);
            } else {
                final String username = user.getUsername();
                if (username == null) {
                    binding.name.setVisibility(View.GONE);
                } else {
                    binding.name.setVisibility(View.VISIBLE);
                    binding.name.setText(username);
                }
            }
        } else {
            binding.name.setVisibility(View.GONE);
        }
    }
}
