package awais.instagrabber.adapters.viewholder.directmessages;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableList;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.customviews.DirectItemContextMenu;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmTextBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

public class DirectItemTextViewHolder extends DirectItemViewHolder {

    private final LayoutDmTextBinding binding;

    public DirectItemTextViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                    @NonNull final LayoutDmTextBinding binding,
                                    final User currentUser,
                                    final DirectThread thread,
                                    @NonNull final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem directItemModel, final MessageDirection messageDirection) {
        final String text = directItemModel.getText();
        if (text == null) return;
        binding.tvMessage.setText(text);
        setupRamboTextListeners(binding.tvMessage);
    }

    @Override
    protected boolean showBackground() {
        return true;
    }

    @Override
    protected List<DirectItemContextMenu.MenuItem> getLongClickOptions() {
        return ImmutableList.of(
                new DirectItemContextMenu.MenuItem(R.id.copy, R.string.copy, item -> {
                    if (TextUtils.isEmpty(item.getText())) return null;
                    Utils.copyText(itemView.getContext(), item.getText());
                    return null;
                })
        );
    }
}
