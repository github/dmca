package awais.instagrabber.adapters.viewholder.directmessages;

import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;

import com.facebook.drawee.backends.pipeline.Fresco;

import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.databinding.LayoutDmAnimatedMediaBinding;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemXma;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.NullSafePair;
import awais.instagrabber.utils.NumberUtils;

public class DirectItemXmaViewHolder extends DirectItemViewHolder {

    private final LayoutDmAnimatedMediaBinding binding;

    public DirectItemXmaViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
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
        final DirectItemXma xma = item.getXma();
        final DirectItemXma.XmaUrlInfo playableUrlInfo = xma.getPlayableUrlInfo();
        final DirectItemXma.XmaUrlInfo previewUrlInfo = xma.getPreviewUrlInfo();
        if (playableUrlInfo == null && previewUrlInfo == null) {
            binding.ivAnimatedMessage.setController(null);
            return;
        }
        final DirectItemXma.XmaUrlInfo urlInfo = playableUrlInfo != null ? playableUrlInfo : previewUrlInfo;
        final String url = urlInfo.getUrl();
        final NullSafePair<Integer, Integer> widthHeight = NumberUtils.calculateWidthHeight(
                urlInfo.getHeight(),
                urlInfo.getWidth(),
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
}
