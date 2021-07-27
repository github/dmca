package awais.instagrabber.adapters.viewholder.directmessages;

import android.view.View;

import androidx.annotation.Nullable;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectReactionsAdapter.OnReactionClickListener;
import awais.instagrabber.customviews.emoji.Emoji;
import awais.instagrabber.databinding.LayoutDmUserItemBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItemEmojiReaction;
import awais.instagrabber.utils.emoji.EmojiParser;

public class DirectReactionViewHolder extends RecyclerView.ViewHolder {
    private final LayoutDmUserItemBinding binding;
    private final long viewerId;
    private final String itemId;
    private final OnReactionClickListener onReactionClickListener;
    private final EmojiParser emojiParser;

    public DirectReactionViewHolder(final LayoutDmUserItemBinding binding,
                                    final long viewerId,
                                    final String itemId,
                                    final OnReactionClickListener onReactionClickListener) {
        super(binding.getRoot());
        this.binding = binding;
        this.viewerId = viewerId;
        this.itemId = itemId;
        this.onReactionClickListener = onReactionClickListener;
        binding.info.setVisibility(View.GONE);
        binding.secondaryImage.setVisibility(View.VISIBLE);
        emojiParser = EmojiParser.getInstance();
    }

    public void bind(final DirectItemEmojiReaction reaction,
                     @Nullable final User user) {
        itemView.setOnClickListener(v -> {
            if (onReactionClickListener == null) return;
            onReactionClickListener.onReactionClick(itemId, reaction);
        });
        setUser(user);
        setReaction(reaction);
    }

    private void setReaction(final DirectItemEmojiReaction reaction) {
        final Emoji emoji = emojiParser.getEmoji(reaction.getEmoji());
        if (emoji == null) {
            binding.secondaryImage.setImageDrawable(null);
            return;
        }
        binding.secondaryImage.setImageDrawable(emoji.getDrawable());
    }

    private void setUser(final User user) {
        if (user == null) {
            binding.fullName.setText("");
            binding.username.setText("");
            binding.profilePic.setImageURI((String) null);
            return;
        }
        binding.fullName.setText(user.getFullName());
        if (user.getPk() == viewerId) {
            binding.username.setText(R.string.tap_to_remove);
        } else {
            binding.username.setText(user.getUsername());
        }
        binding.profilePic.setImageURI(user.getProfilePicUrl());
    }
}
