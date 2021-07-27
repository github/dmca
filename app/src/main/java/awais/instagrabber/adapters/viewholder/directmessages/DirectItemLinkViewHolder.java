package awais.instagrabber.adapters.viewholder.directmessages;

import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;

import com.google.common.collect.ImmutableList;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.customviews.DirectItemContextMenu;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmLinkBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemLink;
import awais.instagrabber.repositories.responses.directmessages.DirectItemLinkContext;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

public class DirectItemLinkViewHolder extends DirectItemViewHolder {

    private final LayoutDmLinkBinding binding;

    public DirectItemLinkViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                    final LayoutDmLinkBinding binding,
                                    final User currentUser,
                                    final DirectThread thread,
                                    final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        final int width = windowWidth - margin - dmRadiusSmall;
        final ViewGroup.LayoutParams layoutParams = binding.preview.getLayoutParams();
        layoutParams.width = width;
        binding.preview.requestLayout();
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem item, final MessageDirection messageDirection) {
        final DirectItemLink link = item.getLink();
        final DirectItemLinkContext linkContext = link.getLinkContext();
        final String linkImageUrl = linkContext.getLinkImageUrl();
        if (TextUtils.isEmpty(linkImageUrl)) {
            binding.preview.setVisibility(View.GONE);
        } else {
            binding.preview.setVisibility(View.VISIBLE);
            binding.preview.setImageURI(linkImageUrl);
        }
        if (TextUtils.isEmpty(linkContext.getLinkTitle())) {
            binding.title.setVisibility(View.GONE);
        } else {
            binding.title.setVisibility(View.VISIBLE);
            binding.title.setText(linkContext.getLinkTitle());
        }
        if (TextUtils.isEmpty(linkContext.getLinkSummary())) {
            binding.summary.setVisibility(View.GONE);
        } else {
            binding.summary.setVisibility(View.VISIBLE);
            binding.summary.setText(linkContext.getLinkSummary());
        }
        if (TextUtils.isEmpty(linkContext.getLinkUrl())) {
            binding.url.setVisibility(View.GONE);
        } else {
            binding.url.setVisibility(View.VISIBLE);
            binding.url.setText(linkContext.getLinkUrl());
        }
        binding.text.setText(link.getText());
        setupListeners(linkContext);
    }

    private void setupListeners(final DirectItemLinkContext linkContext) {
        setupRamboTextListeners(binding.text);
        final View.OnClickListener onClickListener = v -> openURL(linkContext.getLinkUrl());
        binding.preview.setOnClickListener(onClickListener);
        // binding.preview.setOnLongClickListener(v -> itemView.performLongClick());
        binding.title.setOnClickListener(onClickListener);
        binding.summary.setOnClickListener(onClickListener);
        binding.url.setOnClickListener(onClickListener);
    }

    @Override
    protected boolean showBackground() {
        return true;
    }

    @Override
    protected List<DirectItemContextMenu.MenuItem> getLongClickOptions() {
        return ImmutableList.of(
                new DirectItemContextMenu.MenuItem(R.id.copy, R.string.copy, item -> {
                    final DirectItemLink link = item.getLink();
                    if (link == null || TextUtils.isEmpty(link.getText())) return null;
                    Utils.copyText(itemView.getContext(), link.getText());
                    return null;
                })
        );
    }
}
