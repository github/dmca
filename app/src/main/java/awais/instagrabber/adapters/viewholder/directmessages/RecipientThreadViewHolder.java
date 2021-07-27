package awais.instagrabber.adapters.viewholder.directmessages;

import android.content.res.Resources;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.UserSearchResultsAdapter.OnRecipientClickListener;
import awais.instagrabber.databinding.LayoutDmUserItemBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.repositories.responses.directmessages.RankedRecipient;

public class RecipientThreadViewHolder extends RecyclerView.ViewHolder {
    private static final String TAG = RecipientThreadViewHolder.class.getSimpleName();

    private final LayoutDmUserItemBinding binding;
    private final OnRecipientClickListener onThreadClickListener;
    private final float translateAmount;

    public RecipientThreadViewHolder(@NonNull final LayoutDmUserItemBinding binding,
                                     final OnRecipientClickListener onThreadClickListener) {
        super(binding.getRoot());
        this.binding = binding;
        this.onThreadClickListener = onThreadClickListener;
        binding.info.setVisibility(View.GONE);
        final Resources resources = itemView.getResources();
        final int avatarSize = resources.getDimensionPixelSize(R.dimen.dm_inbox_avatar_size);
        translateAmount = ((float) avatarSize) / 7;
    }

    public void bind(final int position,
                     final DirectThread thread,
                     final boolean showSelection,
                     final boolean isSelected) {
        if (thread == null) return;
        binding.getRoot().setOnClickListener(v -> {
            if (onThreadClickListener == null) return;
            onThreadClickListener.onClick(position, RankedRecipient.of(thread), isSelected);
        });
        binding.fullName.setText(thread.getThreadTitle());
        setUsername(thread);
        setProfilePic(thread);
        setSelection(showSelection, isSelected);
    }

    private void setProfilePic(final DirectThread thread) {
        final List<User> users = thread.getUsers();
        binding.profilePic.setImageURI(users.get(0).getProfilePicUrl());
        binding.profilePic.setScaleX(1);
        binding.profilePic.setScaleY(1);
        binding.profilePic.setTranslationX(0);
        binding.profilePic.setTranslationY(0);
        if (users.size() > 1) {
            binding.profilePic2.setVisibility(View.VISIBLE);
            binding.profilePic2.setImageURI(users.get(1).getProfilePicUrl());
            binding.profilePic2.setTranslationX(translateAmount);
            binding.profilePic2.setTranslationY(translateAmount);
            final float scaleAmount = 0.75f;
            binding.profilePic2.setScaleX(scaleAmount);
            binding.profilePic2.setScaleY(scaleAmount);
            binding.profilePic.setScaleX(scaleAmount);
            binding.profilePic.setScaleY(scaleAmount);
            binding.profilePic.setTranslationX(-translateAmount);
            binding.profilePic.setTranslationY(-translateAmount);
            return;
        }
        binding.profilePic2.setVisibility(View.GONE);
    }

    private void setUsername(final DirectThread thread) {
        if (thread.isGroup()) {
            binding.username.setVisibility(View.GONE);
            return;
        }
        binding.username.setVisibility(View.VISIBLE);
        // for a non-group thread, the thread title is the username so set the full name in the username text view
        binding.username.setText(thread.getUsers().get(0).getFullName());
    }

    private void setSelection(final boolean showSelection, final boolean isSelected) {
        binding.select.setVisibility(showSelection ? View.VISIBLE : View.GONE);
        binding.getRoot().setSelected(isSelected);
    }
}
