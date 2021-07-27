package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import awais.instagrabber.adapters.viewholder.HighlightViewHolder;
import awais.instagrabber.databinding.ItemHighlightBinding;
import awais.instagrabber.models.HighlightModel;

public final class HighlightsAdapter extends ListAdapter<HighlightModel, HighlightViewHolder> {

    private final OnHighlightClickListener clickListener;

    private static final DiffUtil.ItemCallback<HighlightModel> diffCallback = new DiffUtil.ItemCallback<HighlightModel>() {
        @Override
        public boolean areItemsTheSame(@NonNull final HighlightModel oldItem, @NonNull final HighlightModel newItem) {
            return oldItem.getId().equals(newItem.getId());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final HighlightModel oldItem, @NonNull final HighlightModel newItem) {
            return oldItem.getId().equals(newItem.getId());
        }
    };

    public HighlightsAdapter(final OnHighlightClickListener clickListener) {
        super(diffCallback);
        this.clickListener = clickListener;
    }

    @NonNull
    @Override
    public HighlightViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemHighlightBinding binding = ItemHighlightBinding.inflate(layoutInflater, parent, false);
        return new HighlightViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull final HighlightViewHolder holder, final int position) {
        final HighlightModel highlightModel = getItem(position);
        if (clickListener != null) {
            holder.itemView.setOnClickListener(v -> clickListener.onHighlightClick(highlightModel, position));
        }
        holder.bind(highlightModel);
    }

    public interface OnHighlightClickListener {
        void onHighlightClick(final HighlightModel model, final int position);
    }
}
