package awais.instagrabber.adapters.viewholder.directmessages;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;

import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmTextBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;

public class DirectItemPlaceholderViewHolder extends DirectItemViewHolder {

    private final LayoutDmTextBinding binding;

    public DirectItemPlaceholderViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                           final LayoutDmTextBinding binding,
                                           final User currentUser,
                                           final DirectThread thread,
                                           final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem directItemModel, final MessageDirection messageDirection) {
        final String text = String.format("%s: %s", directItemModel.getPlaceholder().getTitle(), directItemModel.getPlaceholder().getMessage());
        binding.tvMessage.setText(text);
    }

    @Override
    protected boolean showBackground() {
        return true;
    }

    @Override
    protected boolean allowLongClick() {
        return false;
    }

    @Override
    public int getSwipeDirection() {
        return ItemTouchHelper.ACTION_STATE_IDLE;
    }
}
