package awais.instagrabber.adapters.viewholder;

import android.content.Context;
import android.content.res.Resources;
import android.util.TypedValue;
import android.view.Menu;
import android.view.View;

import androidx.annotation.ColorInt;
import androidx.annotation.NonNull;
import androidx.appcompat.view.ContextThemeWrapper;
import androidx.appcompat.widget.PopupMenu;
import androidx.core.content.ContextCompat;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.R;
import awais.instagrabber.adapters.CommentsAdapter.CommentCallback;
import awais.instagrabber.customviews.ProfilePicView;
import awais.instagrabber.databinding.ItemCommentBinding;
import awais.instagrabber.models.Comment;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Utils;

public final class CommentViewHolder extends RecyclerView.ViewHolder {

    private final ItemCommentBinding binding;
    private final long currentUserId;
    private final CommentCallback commentCallback;
    @ColorInt
    private int parentCommentHighlightColor;
    private PopupMenu optionsPopup;

    public CommentViewHolder(@NonNull final ItemCommentBinding binding,
                             final long currentUserId,
                             final CommentCallback commentCallback) {
        super(binding.getRoot());
        this.binding = binding;
        this.currentUserId = currentUserId;
        this.commentCallback = commentCallback;
        final Context context = itemView.getContext();
        if (context == null) return;
        final Resources.Theme theme = context.getTheme();
        if (theme == null) return;
        final TypedValue typedValue = new TypedValue();
        final boolean resolved = theme.resolveAttribute(R.attr.parentCommentHighlightColor, typedValue, true);
        if (resolved) {
            parentCommentHighlightColor = typedValue.data;
        }
    }

    public void bind(final Comment comment, final boolean isReplyParent, final boolean isReply) {
        if (comment == null) return;
        itemView.setOnClickListener(v -> {
            if (commentCallback != null) {
                commentCallback.onClick(comment);
            }
        });
        if (isReplyParent && parentCommentHighlightColor != 0) {
            itemView.setBackgroundColor(parentCommentHighlightColor);
        } else {
            itemView.setBackgroundColor(itemView.getResources().getColor(android.R.color.transparent));
        }
        setupCommentText(comment, isReply);
        binding.date.setText(comment.getDateTime());
        setLikes(comment, isReply);
        setReplies(comment, isReply);
        setUser(comment, isReply);
        setupOptions(comment, isReply);
    }

    private void setupCommentText(@NonNull final Comment comment, final boolean isReply) {
        binding.comment.clearOnURLClickListeners();
        binding.comment.clearOnHashtagClickListeners();
        binding.comment.clearOnMentionClickListeners();
        binding.comment.clearOnEmailClickListeners();
        binding.comment.setText(comment.getText());
        binding.comment.setTextSize(TypedValue.COMPLEX_UNIT_SP, isReply ? 12 : 14);
        binding.comment.addOnHashtagListener(autoLinkItem -> {
            final String originalText = autoLinkItem.getOriginalText();
            if (commentCallback == null) return;
            commentCallback.onHashtagClick(originalText);
        });
        binding.comment.addOnMentionClickListener(autoLinkItem -> {
            final String originalText = autoLinkItem.getOriginalText();
            if (commentCallback == null) return;
            commentCallback.onMentionClick(originalText);

        });
        binding.comment.addOnEmailClickListener(autoLinkItem -> {
            final String originalText = autoLinkItem.getOriginalText();
            if (commentCallback == null) return;
            commentCallback.onEmailClick(originalText);
        });
        binding.comment.addOnURLClickListener(autoLinkItem -> {
            final String originalText = autoLinkItem.getOriginalText();
            if (commentCallback == null) return;
            commentCallback.onURLClick(originalText);
        });
        binding.comment.setOnLongClickListener(v -> {
            Utils.copyText(itemView.getContext(), comment.getText());
            return true;
        });
        binding.comment.setOnClickListener(v -> commentCallback.onClick(comment));
    }

    private void setUser(@NonNull final Comment comment, final boolean isReply) {
        final User user = comment.getUser();
        if (user == null) return;
        binding.username.setUsername(user.getUsername(), user.isVerified());
        binding.username.setTextAppearance(itemView.getContext(), isReply ? R.style.TextAppearance_MaterialComponents_Subtitle2
                                                                          : R.style.TextAppearance_MaterialComponents_Subtitle1);
        binding.username.setOnClickListener(v -> {
            if (commentCallback == null) return;
            commentCallback.onMentionClick("@" + user.getUsername());
        });
        binding.profilePic.setImageURI(user.getProfilePicUrl());
        binding.profilePic.setSize(isReply ? ProfilePicView.Size.SMALLER : ProfilePicView.Size.SMALL);
        binding.profilePic.setOnClickListener(v -> {
            if (commentCallback == null) return;
            commentCallback.onMentionClick("@" + user.getUsername());
        });
    }

    private void setLikes(@NonNull final Comment comment, final boolean isReply) {
        // final String likesString = itemView.getResources().getQuantityString(R.plurals.likes_count, likes, likes);
        binding.likes.setText(String.valueOf(comment.getLikes()));
        binding.likes.setOnLongClickListener(v -> {
            if (commentCallback == null) return false;
            commentCallback.onViewLikes(comment);
            return true;
        });
        if (currentUserId == 0) { // not logged in
            binding.likes.setOnClickListener(v -> {
                if (commentCallback == null) return;
                commentCallback.onViewLikes(comment);
            });
            return;
        }
        final boolean liked = comment.getLiked();
        final int resId = liked ? R.drawable.ic_like : R.drawable.ic_not_liked;
        binding.likes.setCompoundDrawablesRelativeWithSize(ContextCompat.getDrawable(itemView.getContext(), resId), null, null, null);
        binding.likes.setOnClickListener(v -> {
            if (commentCallback == null) return;
            // toggle like
            commentCallback.onLikeClick(comment, !liked, isReply);
        });
    }

    private void setReplies(@NonNull final Comment comment, final boolean isReply) {
        final int replies = comment.getReplyCount();
        binding.replies.setVisibility(View.VISIBLE);
        final String text = isReply ? "" : String.valueOf(replies);
        // final String string = itemView.getResources().getQuantityString(R.plurals.replies_count, replies, replies);
        binding.replies.setText(text);
        binding.replies.setOnClickListener(v -> {
            if (commentCallback == null) return;
            commentCallback.onRepliesClick(comment);
        });
    }

    private void setupOptions(final Comment comment, final boolean isReply) {
        binding.options.setOnClickListener(v -> {
            if (optionsPopup == null) {
                createOptionsPopupMenu(comment, isReply);
            }
            if (optionsPopup == null) return;
            optionsPopup.show();
        });
    }

    private void createOptionsPopupMenu(final Comment comment, final boolean isReply) {
        if (optionsPopup == null) {
            final ContextThemeWrapper themeWrapper = new ContextThemeWrapper(itemView.getContext(), R.style.popupMenuStyle);
            optionsPopup = new PopupMenu(themeWrapper, binding.options);
        } else {
            optionsPopup.getMenu().clear();
        }
        optionsPopup.getMenuInflater().inflate(R.menu.comment_options_menu, optionsPopup.getMenu());
        final User user = comment.getUser();
        if (currentUserId == 0 || user == null || user.getPk() != currentUserId) {
            final Menu menu = optionsPopup.getMenu();
            menu.removeItem(R.id.delete);
        }
        optionsPopup.setOnMenuItemClickListener(item -> {
            if (commentCallback == null) return false;
            int itemId = item.getItemId();
            if (itemId == R.id.translate) {
                commentCallback.onTranslate(comment);
                return true;
            }
            if (itemId == R.id.delete) {
                commentCallback.onDelete(comment, isReply);
            }
            return true;
        });
    }

    // private void setupReply(final Comment comment) {
    //     if (!isLoggedIn) {
    //         binding.reply.setVisibility(View.GONE);
    //         return;
    //     }
    //     binding.reply.setOnClickListener(v -> {
    //         if (commentCallback == null) return;
    //         // toggle like
    //         commentCallback.onReplyClick(comment);
    //     });
    // }
}