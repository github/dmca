package awais.instagrabber.adapters.viewholder;

import android.annotation.SuppressLint;
import android.content.res.ColorStateList;
import android.graphics.drawable.Drawable;
import android.view.MotionEvent;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.core.widget.ImageViewCompat;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.color.MaterialColors;

import awais.instagrabber.R;
import awais.instagrabber.adapters.TabsAdapter;
import awais.instagrabber.databinding.ItemTabOrderPrefBinding;
import awais.instagrabber.models.Tab;

public class TabViewHolder extends RecyclerView.ViewHolder {
    private final ItemTabOrderPrefBinding binding;
    private final TabsAdapter.TabAdapterCallback tabAdapterCallback;
    private final int highlightColor;
    private final Drawable originalBgColor;

    private boolean draggable = true;

    @SuppressLint("ClickableViewAccessibility")
    public TabViewHolder(@NonNull final ItemTabOrderPrefBinding binding,
                         @NonNull final TabsAdapter.TabAdapterCallback tabAdapterCallback) {
        super(binding.getRoot());
        this.binding = binding;
        this.tabAdapterCallback = tabAdapterCallback;
        highlightColor = MaterialColors.getColor(itemView.getContext(), R.attr.colorControlHighlight, 0);
        originalBgColor = itemView.getBackground();
        binding.handle.setOnTouchListener((v, event) -> {
            if (event.getActionMasked() == MotionEvent.ACTION_DOWN) {
                tabAdapterCallback.onStartDrag(this);
            }
            return true;
        });
    }

    public void bind(@NonNull final Tab tab,
                     final boolean isInOthers,
                     final boolean isCurrentFull) {
        draggable = !isInOthers;
        binding.icon.setImageResource(tab.getIconResId());
        binding.title.setText(tab.getTitle());
        binding.handle.setVisibility(isInOthers ? View.GONE : View.VISIBLE);
        binding.addRemove.setImageResource(isInOthers ? R.drawable.ic_round_add_circle_24
                                                      : R.drawable.ic_round_remove_circle_24);
        final ColorStateList tintList = ColorStateList.valueOf(ContextCompat.getColor(
                itemView.getContext(),
                isInOthers ? R.color.green_500
                           : R.color.red_500));
        ImageViewCompat.setImageTintList(binding.addRemove, tintList);
        binding.addRemove.setOnClickListener(v -> {
            if (isInOthers) {
                tabAdapterCallback.onAdd(tab);
                return;
            }
            tabAdapterCallback.onRemove(tab);
        });
        final boolean enabled = tab.isRemovable()
                && !(isInOthers && isCurrentFull); // All slots are full in current
        binding.addRemove.setEnabled(enabled);
        binding.addRemove.setAlpha(enabled ? 1 : 0.5F);
    }

    public boolean isDraggable() {
        return draggable;
    }

    public void setDragging(final boolean isDragging) {
        if (isDragging) {
            if (highlightColor != 0) {
                itemView.setBackgroundColor(highlightColor);
            } else {
                itemView.setAlpha(0.5F);
            }
            return;
        }
        itemView.setAlpha(1);
        itemView.setBackground(originalBgColor);
    }
}
