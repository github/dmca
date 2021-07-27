package awais.instagrabber.adapters.viewholder.directmessages;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.ItemTouchHelper;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmTextBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;

public class DirectItemDefaultViewHolder extends DirectItemViewHolder {

    private final LayoutDmTextBinding binding;

    public DirectItemDefaultViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                       @NonNull final LayoutDmTextBinding binding,
                                       final User currentUser,
                                       final DirectThread thread,
                                       final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem directItemModel, final MessageDirection messageDirection) {
        final Context context = itemView.getContext();
        binding.tvMessage.setText(context.getText(R.string.dms_inbox_raven_message_unknown));
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
