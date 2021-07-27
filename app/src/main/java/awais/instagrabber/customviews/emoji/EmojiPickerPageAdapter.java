package awais.instagrabber.customviews.emoji;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.AdapterListUpdateCallback;
import androidx.recyclerview.widget.AsyncDifferConfig;
import androidx.recyclerview.widget.AsyncListDiffer;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.customviews.emoji.EmojiPicker.OnEmojiClickListener;
import awais.instagrabber.customviews.helpers.GridSpacingItemDecoration;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.utils.emoji.EmojiParser;

import static android.view.ViewGroup.LayoutParams.MATCH_PARENT;

public class EmojiPickerPageAdapter extends RecyclerView.Adapter<EmojiCategoryPageViewHolder> {

    private static final DiffUtil.ItemCallback<EmojiCategory> diffCallback = new DiffUtil.ItemCallback<EmojiCategory>() {
        @Override
        public boolean areItemsTheSame(@NonNull final EmojiCategory oldItem, @NonNull final EmojiCategory newItem) {
            return oldItem.equals(newItem);
        }

        @Override
        public boolean areContentsTheSame(@NonNull final EmojiCategory oldItem, @NonNull final EmojiCategory newItem) {
            return oldItem.equals(newItem);
        }
    };

    private final View rootView;
    private final OnEmojiClickListener onEmojiClickListener;
    private final AsyncListDiffer<EmojiCategory> differ;

    public EmojiPickerPageAdapter(final View rootView,
                                  final OnEmojiClickListener onEmojiClickListener) {
        this.rootView = rootView;
        this.onEmojiClickListener = onEmojiClickListener;
        differ = new AsyncListDiffer<>(new AdapterListUpdateCallback(this),
                                       new AsyncDifferConfig.Builder<>(diffCallback).build());
        differ.submitList(EmojiParser.getInstance().getEmojiCategories());
        setHasStableIds(true);
    }

    @NonNull
    @Override
    public EmojiCategoryPageViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final Context context = parent.getContext();
        final RecyclerView emojiGrid = new RecyclerView(context);
        emojiGrid.setLayoutParams(new ViewGroup.LayoutParams(MATCH_PARENT, MATCH_PARENT));
        emojiGrid.setLayoutManager(new GridLayoutManager(context, 9));
        emojiGrid.setHasFixedSize(true);
        emojiGrid.setClipToPadding(false);
        emojiGrid.addItemDecoration(new GridSpacingItemDecoration(Utils.convertDpToPx(8)));
        return new EmojiCategoryPageViewHolder(rootView, emojiGrid, onEmojiClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final EmojiCategoryPageViewHolder holder, final int position) {
        final EmojiCategory emojiCategory = differ.getCurrentList().get(position);
        holder.bind(emojiCategory);
    }

    @Override
    public long getItemId(final int position) {
        return differ.getCurrentList().get(position).hashCode();
    }

    @Override
    public int getItemCount() {
        return differ.getCurrentList().size();
    }
}
