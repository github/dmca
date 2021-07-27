package awais.instagrabber.adapters.viewholder.directmessages;

import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;

import com.facebook.drawee.backends.pipeline.Fresco;
import com.google.common.collect.ImmutableList;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.customviews.DirectItemContextMenu;
import awais.instagrabber.databinding.LayoutDmAnimatedMediaBinding;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.repositories.responses.AnimatedMediaFixedHeight;
import awais.instagrabber.repositories.responses.AnimatedMediaImages;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemAnimatedMedia;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.NullSafePair;
import awais.instagrabber.utils.NumberUtils;
import awais.instagrabber.utils.Utils;

public class DirectItemAnimatedMediaViewHolder extends DirectItemViewHolder {

    private final LayoutDmAnimatedMediaBinding binding;

    public DirectItemAnimatedMediaViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                             @NonNull final LayoutDmAnimatedMediaBinding binding,
                                             final User currentUser,
                                             final DirectThread thread,
                                             final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem item, final MessageDirection messageDirection) {
        final DirectItemAnimatedMedia animatedMediaModel = item.getAnimatedMedia();
        final AnimatedMediaImages images = animatedMediaModel.getImages();
        if (images == null) return;
        final AnimatedMediaFixedHeight fixedHeight = images.getFixedHeight();
        if (fixedHeight == null) return;
        final String url = fixedHeight.getWebp();
        final NullSafePair<Integer, Integer> widthHeight = NumberUtils.calculateWidthHeight(
                fixedHeight.getHeight(),
                fixedHeight.getWidth(),
                mediaImageMaxHeight,
                mediaImageMaxWidth
        );
        binding.ivAnimatedMessage.setVisibility(View.VISIBLE);
        final ViewGroup.LayoutParams layoutParams = binding.ivAnimatedMessage.getLayoutParams();
        final int width = widthHeight.first;
        final int height = widthHeight.second;
        layoutParams.width = width;
        layoutParams.height = height;
        binding.ivAnimatedMessage.requestLayout();
        binding.ivAnimatedMessage.setController(Fresco.newDraweeControllerBuilder()
                                                      .setUri(url)
                                                      .setAutoPlayAnimations(true)
                                                      .build());
    }

    @Override
    public int getSwipeDirection() {
        return ItemTouchHelper.ACTION_STATE_IDLE;
    }

    @Override
    protected List<DirectItemContextMenu.MenuItem> getLongClickOptions() {
        return ImmutableList.of(
                new DirectItemContextMenu.MenuItem(R.id.detail, R.string.dms_inbox_giphy, item -> {
                    Utils.openURL(itemView.getContext(), "https://giphy.com/gifs/" + item.getAnimatedMedia().getId());
                    return null;
                })
        );
    }
}
