package awais.instagrabber.adapters;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.List;

public abstract class MultiSelectListAdapter<T extends MultiSelectListAdapter.Selectable, VH extends RecyclerView.ViewHolder> extends
        ListAdapter<T, VH> {

    private boolean isSelecting;
    private OnItemClickListener<T> internalOnItemClickListener;
    private OnItemLongClickListener<T> internalOnLongItemClickListener;

    private final List<T> selectedItems = new ArrayList<>();

    protected MultiSelectListAdapter(@NonNull final DiffUtil.ItemCallback<T> diffCallback,
                                     final OnItemClickListener<T> clickListener,
                                     final OnItemLongClickListener<T> longClickListener) {
        super(diffCallback);
        internalOnItemClickListener = (item, position) -> {
            if (isSelecting) {
                toggleSelection(item, position);
            }
            if (clickListener == null) {
                return;
            }
            clickListener.onItemClick(item, position);
        };
        internalOnLongItemClickListener = (item, position) -> {
            if (!isSelecting) {
                isSelecting = true;
            }
            toggleSelection(item, position);
            if (longClickListener == null) {
                return true;
            }
            return longClickListener.onItemLongClick(item, position);
        };
    }

    public OnItemClickListener<T> getInternalOnItemClickListener() {
        return internalOnItemClickListener;
    }

    public OnItemLongClickListener<T> getInternalOnLongItemClickListener() {
        return internalOnLongItemClickListener;
    }

    private void toggleSelection(final T item, final int position) {
        if (item == null) {
            return;
        }
        if (selectedItems.size() >= 100) {
            // Toast.makeText(mainActivity, R.string.downloader_too_many, Toast.LENGTH_SHORT);
            return;
        }
        if (item.isSelected()) {
            item.setSelected(false);
            selectedItems.remove(item);
        } else {
            item.setSelected(true);
            selectedItems.add(item);
        }
        if (selectedItems.size() == 0) {
            isSelecting = false;
        }
        notifyItemChanged(position);
    }

    public boolean isSelecting() {
        return isSelecting;
    }

    public List<T> getSelectedModels() {
        return selectedItems;
    }

    public void clearSelection() {
        for (final T item : selectedItems) {
            item.setSelected(false);
        }
        selectedItems.clear();
        isSelecting = false;
        notifyDataSetChanged();
    }

    public interface Selectable {
        boolean isSelected();

        void setSelected(boolean selected);
    }

    public interface OnItemClickListener<T> {
        void onItemClick(T item, int position);
    }

    public interface OnItemLongClickListener<T> {
        boolean onItemLongClick(T item, int position);
    }
}
