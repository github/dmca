package awais.instagrabber.customviews.emoji;

import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.customviews.emoji.EmojiPicker.OnEmojiClickListener;

public class EmojiCategoryPageViewHolder extends RecyclerView.ViewHolder {
    // private static final String TAG = EmojiCategoryPageViewHolder.class.getSimpleName();

    private final View rootView;
    private final OnEmojiClickListener onEmojiClickListener;

    public EmojiCategoryPageViewHolder(@NonNull final View rootView,
                                       @NonNull final RecyclerView itemView,
                                       final OnEmojiClickListener onEmojiClickListener) {
        super(itemView);
        this.rootView = rootView;
        this.onEmojiClickListener = onEmojiClickListener;
    }

    public void bind(final EmojiCategory emojiCategory) {
        final RecyclerView emojiGrid = (RecyclerView) itemView;
        final EmojiGridAdapter adapter = new EmojiGridAdapter(
                emojiCategory.getType(),
                onEmojiClickListener,
                (position, view, parent) -> {
                    final EmojiVariantPopup emojiVariantPopup = new EmojiVariantPopup(rootView, ((view1, emoji) -> {
                        if (onEmojiClickListener != null) {
                            onEmojiClickListener.onClick(view1, emoji);
                        }
                        final EmojiGridAdapter emojiGridAdapter = (EmojiGridAdapter) emojiGrid.getAdapter();
                        if (emojiGridAdapter == null) return;
                        emojiGridAdapter.notifyItemChanged(position);
                    }));
                    emojiVariantPopup.show(view, parent);
                    return true;
                }
        );
        emojiGrid.setAdapter(adapter);
    }
}
