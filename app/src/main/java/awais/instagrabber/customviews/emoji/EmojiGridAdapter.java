package awais.instagrabber.customviews.emoji;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.AdapterListUpdateCallback;
import androidx.recyclerview.widget.AsyncDifferConfig;
import androidx.recyclerview.widget.AsyncListDiffer;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.RecyclerView;

import com.google.common.collect.ImmutableList;

import java.util.Collections;
import java.util.Map;
import java.util.Optional;

import awais.instagrabber.customviews.emoji.EmojiPicker.OnEmojiClickListener;
import awais.instagrabber.databinding.ItemEmojiGridBinding;
import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.emoji.EmojiParser;

public class EmojiGridAdapter extends RecyclerView.Adapter<EmojiGridAdapter.EmojiViewHolder> {
    private static final String TAG = EmojiGridAdapter.class.getSimpleName();

    private static final DiffUtil.ItemCallback<Emoji> diffCallback = new DiffUtil.ItemCallback<Emoji>() {
        @Override
        public boolean areItemsTheSame(@NonNull final Emoji oldItem, @NonNull final Emoji newItem) {
            return oldItem.equals(newItem);
        }

        @Override
        public boolean areContentsTheSame(@NonNull final Emoji oldItem, @NonNull final Emoji newItem) {
            return oldItem.equals(newItem);
        }
    };

    private final AsyncListDiffer<Emoji> differ;
    private final OnEmojiLongClickListener onEmojiLongClickListener;
    private final OnEmojiClickListener onEmojiClickListener;
    private final EmojiVariantManager emojiVariantManager;
    private final AppExecutors appExecutors;

    public EmojiGridAdapter(final EmojiCategoryType emojiCategoryType,
                            final OnEmojiClickListener onEmojiClickListener,
                            final OnEmojiLongClickListener onEmojiLongClickListener) {
        this.onEmojiClickListener = onEmojiClickListener;
        this.onEmojiLongClickListener = onEmojiLongClickListener;
        differ = new AsyncListDiffer<>(new AdapterListUpdateCallback(this),
                                       new AsyncDifferConfig.Builder<>(diffCallback).build());
        final EmojiParser emojiParser = EmojiParser.getInstance();
        final Map<EmojiCategoryType, EmojiCategory> categoryMap = emojiParser.getCategoryMap();
        emojiVariantManager = EmojiVariantManager.getInstance();
        appExecutors = AppExecutors.getInstance();
        setHasStableIds(true);
        if (emojiCategoryType == null) {
            // show all if type is null
            differ.submitList(ImmutableList.copyOf(emojiParser.getAllEmojis().values()));
            return;
        }
        final EmojiCategory emojiCategory = categoryMap.get(emojiCategoryType);
        if (emojiCategory == null) {
            differ.submitList(Collections.emptyList());
            return;
        }
        differ.submitList(ImmutableList.copyOf(emojiCategory.getEmojis().values()));
    }

    @NonNull
    @Override
    public EmojiViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemEmojiGridBinding binding = ItemEmojiGridBinding.inflate(layoutInflater, parent, false);
        return new EmojiViewHolder(binding, onEmojiClickListener, onEmojiLongClickListener);
    }

    @Override
    public void onBindViewHolder(@NonNull final EmojiViewHolder holder, final int position) {
        final Emoji emoji = differ.getCurrentList().get(position);
        final String variant = emojiVariantManager.getVariant(emoji.getUnicode());
        if (variant != null) {
            appExecutors.tasksThread().execute(() -> {
                final Optional<Emoji> first = emoji.getVariants()
                                                   .stream()
                                                   .filter(e -> e.getUnicode().equals(variant))
                                                   .findFirst();
                if (!first.isPresent()) return;
                appExecutors.mainThread().execute(() -> holder.bind(position, first.get(), emoji));
            });
            return;
        }
        holder.bind(position, emoji, emoji);
    }

    @Override
    public long getItemId(final int position) {
        return differ.getCurrentList().get(position).hashCode();
    }

    @Override
    public int getItemViewType(final int position) {
        return 0;
    }

    @Override
    public int getItemCount() {
        return differ.getCurrentList().size();
    }

    public static class EmojiViewHolder extends RecyclerView.ViewHolder {
        // private final AppExecutors appExecutors = AppExecutors.getInstance();
        private final ItemEmojiGridBinding binding;
        private final OnEmojiClickListener onEmojiClickListener;
        private final OnEmojiLongClickListener onEmojiLongClickListener;

        public EmojiViewHolder(@NonNull final ItemEmojiGridBinding binding,
                               final OnEmojiClickListener onEmojiClickListener,
                               final OnEmojiLongClickListener onEmojiLongClickListener) {
            super(binding.getRoot());
            this.binding = binding;
            this.onEmojiClickListener = onEmojiClickListener;
            this.onEmojiLongClickListener = onEmojiLongClickListener;
        }

        public void bind(final int position, final Emoji emoji, final Emoji parent) {
            binding.image.setImageDrawable(null);
            binding.indicator.setVisibility(View.GONE);
            itemView.setOnLongClickListener(null);
            // itemView.post(() -> {
            binding.image.setImageDrawable(emoji.getDrawable());
            final boolean hasVariants = !parent.getVariants().isEmpty();
            binding.indicator.setVisibility(hasVariants ? View.VISIBLE : View.GONE);
            if (onEmojiClickListener != null) {
                itemView.setOnClickListener(v -> onEmojiClickListener.onClick(v, emoji));
            }
            if (hasVariants && onEmojiLongClickListener != null) {
                itemView.setOnLongClickListener(v -> onEmojiLongClickListener.onLongClick(position, v, parent));
            }
            // });
        }
    }

    public interface OnEmojiLongClickListener {
        boolean onLongClick(int position, View view, Emoji emoji);
    }
}
