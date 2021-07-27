package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import java.util.Objects;

import awais.instagrabber.adapters.viewholder.CommentViewHolder;
import awais.instagrabber.databinding.ItemCommentBinding;
import awais.instagrabber.models.Comment;

public final class CommentsAdapter extends ListAdapter<Comment, CommentViewHolder> {
    private static final DiffUtil.ItemCallback<Comment> DIFF_CALLBACK = new DiffUtil.ItemCallback<Comment>() {
        @Override
        public boolean areItemsTheSame(@NonNull final Comment oldItem, @NonNull final Comment newItem) {
            return Objects.equals(oldItem.getId(), newItem.getId());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final Comment oldItem, @NonNull final Comment newItem) {
            return Objects.equals(oldItem, newItem);
        }
    };

    private final boolean showingReplies;
    private final CommentCallback commentCallback;
    private final long currentUserId;

    public CommentsAdapter(final long currentUserId,
                           final boolean showingReplies,
                           final CommentCallback commentCallback) {
        super(DIFF_CALLBACK);
        this.showingReplies = showingReplies;
        this.currentUserId = currentUserId;
        this.commentCallback = commentCallback;
    }

    @NonNull
    @Override
    public CommentViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int type) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemCommentBinding binding = ItemCommentBinding.inflate(layoutInflater, parent, false);
        return new CommentViewHolder(binding, currentUserId, commentCallback);
    }

    @Override
    public void onBindViewHolder(@NonNull final CommentViewHolder holder, final int position) {
        final Comment comment = getItem(position);
        holder.bind(comment, showingReplies && position == 0, showingReplies && position != 0);
    }

    public interface CommentCallback {
        void onClick(final Comment comment);

        void onHashtagClick(final String hashtag);

        void onMentionClick(final String mention);

        void onURLClick(final String url);

        void onEmailClick(final String emailAddress);

        void onLikeClick(final Comment comment, boolean liked, final boolean isReply);

        void onRepliesClick(final Comment comment);

        void onViewLikes(Comment comment);

        void onTranslate(Comment comment);

        void onDelete(Comment comment, boolean isReply);
    }
}