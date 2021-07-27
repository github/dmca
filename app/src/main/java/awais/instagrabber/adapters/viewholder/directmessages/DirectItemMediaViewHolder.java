package awais.instagrabber.adapters.viewholder.directmessages;

import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;

import com.facebook.drawee.drawable.ScalingUtils;
import com.facebook.drawee.generic.GenericDraweeHierarchyBuilder;
import com.facebook.drawee.generic.RoundingParams;

import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmMediaBinding;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.NullSafePair;
import awais.instagrabber.utils.NumberUtils;
import awais.instagrabber.utils.ResponseBodyUtils;

public class DirectItemMediaViewHolder extends DirectItemViewHolder {

    private final LayoutDmMediaBinding binding;
    private final RoundingParams incomingRoundingParams;
    private final RoundingParams outgoingRoundingParams;

    public DirectItemMediaViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                     @NonNull final LayoutDmMediaBinding binding,
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
    public void bindItem(final DirectItem directItemModel, final MessageDirection messageDirection) {
        final RoundingParams roundingParams = messageDirection == MessageDirection.INCOMING ? incomingRoundingParams : outgoingRoundingParams;
        binding.mediaPreview.setHierarchy(new GenericDraweeHierarchyBuilder(itemView.getResources())
                                                  .setRoundingParams(roundingParams)
                                                  .setActualImageScaleType(ScalingUtils.ScaleType.CENTER_CROP)
                                                  .build());
        final Media media = directItemModel.getMedia();
        itemView.setOnClickListener(v -> openMedia(media));
        final MediaItemType modelMediaType = media.getMediaType();
        binding.typeIcon.setVisibility(modelMediaType == MediaItemType.MEDIA_TYPE_VIDEO || modelMediaType == MediaItemType.MEDIA_TYPE_SLIDER
                                       ? View.VISIBLE
                                       : View.GONE);
        final NullSafePair<Integer, Integer> widthHeight = NumberUtils.calculateWidthHeight(
                media.getOriginalHeight(),
                media.getOriginalWidth(),
                mediaImageMaxHeight,
                mediaImageMaxWidth
        );
        final ViewGroup.LayoutParams layoutParams = binding.mediaPreview.getLayoutParams();
        final int width = widthHeight.first;
        layoutParams.width = width;
        layoutParams.height = widthHeight.second;
        binding.mediaPreview.requestLayout();
        binding.bgTime.getLayoutParams().width = width;
        binding.bgTime.requestLayout();
        final String thumbUrl = ResponseBodyUtils.getThumbUrl(media);
        binding.mediaPreview.setImageURI(thumbUrl);
    }

}
