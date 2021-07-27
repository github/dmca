package awais.instagrabber.adapters.viewholder.directmessages;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;

import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmLikeBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;

public class DirectItemLikeViewHolder extends DirectItemViewHolder {

    public DirectItemLikeViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                    @NonNull final LayoutDmLikeBinding binding,
                                    final User currentUser,
                                    final DirectThread thread,
                                    final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem directItemModel, final MessageDirection messageDirection) {}

    @Override
    protected boolean canForward() {
        return false;
    }

    @Override
    public int getSwipeDirection() {
        return ItemTouchHelper.ACTION_STATE_IDLE;
    }
}
