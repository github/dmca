package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.graphics.Canvas;
import android.os.Bundle;
import android.util.Pair;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.DialogFragment;
import androidx.recyclerview.widget.ItemTouchHelper;
import androidx.recyclerview.widget.ItemTouchHelper.SimpleCallback;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.common.collect.ImmutableList;

import java.util.List;
import java.util.stream.Collectors;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectUsersAdapter;
import awais.instagrabber.adapters.TabsAdapter;
import awais.instagrabber.adapters.viewholder.TabViewHolder;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.models.Tab;
import awais.instagrabber.utils.Utils;

import static androidx.recyclerview.widget.ItemTouchHelper.ACTION_STATE_DRAG;
import static androidx.recyclerview.widget.ItemTouchHelper.DOWN;
import static androidx.recyclerview.widget.ItemTouchHelper.UP;

public class TabOrderPreferenceDialogFragment extends DialogFragment {
    private Callback callback;
    private Context context;
    private List<Tab> tabsInPref;
    private ItemTouchHelper itemTouchHelper;
    private AlertDialog dialog;
    private List<Tab> newOrderTabs;
    private List<Tab> newOtherTabs;

    private final TabsAdapter.TabAdapterCallback tabAdapterCallback = new TabsAdapter.TabAdapterCallback() {
        @Override
        public void onStartDrag(final TabViewHolder viewHolder) {
            if (itemTouchHelper == null || viewHolder == null) return;
            itemTouchHelper.startDrag(viewHolder);
        }

        @Override
        public void onOrderChange(final List<Tab> newOrderTabs) {
            if (newOrderTabs == null || tabsInPref == null || dialog == null) return;
            TabOrderPreferenceDialogFragment.this.newOrderTabs = newOrderTabs;
            setSaveButtonState(newOrderTabs);
        }

        @Override
        public void onAdd(final Tab tab) {
            // Add this tab to newOrderTabs
            newOrderTabs = ImmutableList.<Tab>builder()
                    .addAll(newOrderTabs)
                    .add(tab)
                    .build();
            // Remove this tab from newOtherTabs
            if (newOtherTabs != null) {
                newOtherTabs = newOtherTabs.stream()
                                           .filter(t -> !t.equals(tab))
                                           .collect(Collectors.toList());
            }
            setSaveButtonState(newOrderTabs);
            // submit these tab lists to adapter
            if (adapter == null) return;
            adapter.submitList(newOrderTabs, newOtherTabs, () -> list.postDelayed(() -> adapter.notifyDataSetChanged(), 300));
        }

        @Override
        public void onRemove(final Tab tab) {
            // Remove this tab from newOrderTabs
            newOrderTabs = newOrderTabs.stream()
                                       .filter(t -> !t.equals(tab))
                                       .collect(Collectors.toList());
            // Add this tab to newOtherTabs
            if (newOtherTabs != null) {
                newOtherTabs = ImmutableList.<Tab>builder()
                        .addAll(newOtherTabs)
                        .add(tab)
                        .build();
            }
            setSaveButtonState(newOrderTabs);
            // submit these tab lists to adapter
            if (adapter == null) return;
            adapter.submitList(newOrderTabs, newOtherTabs, () -> list.postDelayed(() -> {
                adapter.notifyDataSetChanged();
                if (tab.getNavigationRootId() == R.id.direct_messages_nav_graph) {
                    final ConfirmDialogFragment dialogFragment = ConfirmDialogFragment.newInstance(
                            111, 0, R.string.dm_remove_warning, R.string.ok, 0, 0
                    );
                    dialogFragment.show(getChildFragmentManager(), "dm_warning_dialog");
                }
            }, 500));
        }

        private void setSaveButtonState(final List<Tab> newOrderTabs) {
            dialog.getButton(AlertDialog.BUTTON_POSITIVE)
                  .setEnabled(!newOrderTabs.equals(tabsInPref));
        }
    };
    private final SimpleCallback simpleCallback = new SimpleCallback(UP | DOWN, 0) {
        private int movePosition = RecyclerView.NO_POSITION;

        @Override
        public int getMovementFlags(@NonNull final RecyclerView recyclerView, @NonNull final RecyclerView.ViewHolder viewHolder) {
            if (viewHolder instanceof DirectUsersAdapter.HeaderViewHolder) return 0;
            if (viewHolder instanceof TabViewHolder && !((TabViewHolder) viewHolder).isDraggable()) return 0;
            return super.getMovementFlags(recyclerView, viewHolder);
        }

        @Override
        public void onChildDraw(@NonNull final Canvas c,
                                @NonNull final RecyclerView recyclerView,
                                @NonNull final RecyclerView.ViewHolder viewHolder,
                                final float dX,
                                final float dY,
                                final int actionState,
                                final boolean isCurrentlyActive) {
            if (actionState != ACTION_STATE_DRAG) {
                super.onChildDraw(c, recyclerView, viewHolder, dX, dY, actionState, isCurrentlyActive);
                return;
            }
            final TabsAdapter adapter = (TabsAdapter) recyclerView.getAdapter();
            if (adapter == null) {
                super.onChildDraw(c, recyclerView, viewHolder, dX, dY, actionState, isCurrentlyActive);
                return;
            }
            // Do not allow dragging into 'Other tabs' category
            float edgeY = dY;
            final int lastPosition = adapter.getCurrentCount() - 1;
            final View view = viewHolder.itemView;
            // final int topEdge = recyclerView.getTop();
            final int bottomEdge = view.getHeight() * adapter.getCurrentCount() - view.getBottom();
            // if (movePosition == 0 && dY < topEdge) {
            //     edgeY = topEdge;
            // } else
            if (movePosition >= lastPosition && dY >= bottomEdge) {
                edgeY = bottomEdge;
            }
            super.onChildDraw(c, recyclerView, viewHolder, dX, edgeY, actionState, isCurrentlyActive);
        }

        @Override
        public boolean onMove(@NonNull final RecyclerView recyclerView,
                              @NonNull final RecyclerView.ViewHolder viewHolder,
                              @NonNull final RecyclerView.ViewHolder target) {
            final TabsAdapter adapter = (TabsAdapter) recyclerView.getAdapter();
            if (adapter == null) return false;
            movePosition = target.getBindingAdapterPosition();
            if (movePosition >= adapter.getCurrentCount()) {
                return false;
            }
            final int from = viewHolder.getBindingAdapterPosition();
            final int to = target.getBindingAdapterPosition();
            adapter.moveItem(from, to);
            // adapter.notifyItemMoved(from, to);
            return true;
        }

        @Override
        public void onSwiped(@NonNull final RecyclerView.ViewHolder viewHolder, final int direction) {}

        @Override
        public void onSelectedChanged(@Nullable final RecyclerView.ViewHolder viewHolder, final int actionState) {
            super.onSelectedChanged(viewHolder, actionState);
            if (!(viewHolder instanceof TabViewHolder)) {
                movePosition = RecyclerView.NO_POSITION;
                return;
            }
            if (actionState == ACTION_STATE_DRAG) {
                ((TabViewHolder) viewHolder).setDragging(true);
                movePosition = viewHolder.getBindingAdapterPosition();
            }
        }

        @Override
        public void clearView(@NonNull final RecyclerView recyclerView,
                              @NonNull final RecyclerView.ViewHolder viewHolder) {
            super.clearView(recyclerView, viewHolder);
            ((TabViewHolder) viewHolder).setDragging(false);
            movePosition = RecyclerView.NO_POSITION;
        }
    };
    private TabsAdapter adapter;
    private RecyclerView list;

    public static TabOrderPreferenceDialogFragment newInstance() {
        final Bundle args = new Bundle();
        final TabOrderPreferenceDialogFragment fragment = new TabOrderPreferenceDialogFragment();
        fragment.setArguments(args);
        return fragment;
    }

    public TabOrderPreferenceDialogFragment() {}

    @Override
    public void onAttach(@NonNull final Context context) {
        super.onAttach(context);
        try {
            callback = (Callback) getParentFragment();
        } catch (ClassCastException e) {
            // throw new ClassCastException("Calling fragment must implement TabOrderPreferenceDialogFragment.Callback interface");
        }
        this.context = context;
    }

    @NonNull
    @Override
    public Dialog onCreateDialog(@Nullable final Bundle savedInstanceState) {
        return new MaterialAlertDialogBuilder(context)
                .setView(createView())
                .setPositiveButton(R.string.save, (d, w) -> {
                    final boolean hasChanged = newOrderTabs != null && !newOrderTabs.equals(tabsInPref);
                    if (hasChanged) {
                        saveNewOrder();
                    }
                    if (callback == null) return;
                    callback.onSave(hasChanged);
                })
                .setNegativeButton(R.string.cancel, (dialog, which) -> {
                    if (callback == null) return;
                    callback.onCancel();
                })
                .create();
    }

    private void saveNewOrder() {
        final String newOrderString = newOrderTabs.stream()
                                                  .map(Tab::getGraphName)
                                                  .collect(Collectors.joining(","));
        Utils.settingsHelper.putString(PreferenceKeys.PREF_TAB_ORDER, newOrderString);
    }

    @Override
    public void onStart() {
        super.onStart();
        final Dialog dialog = getDialog();
        if (!(dialog instanceof AlertDialog)) return;
        this.dialog = (AlertDialog) dialog;
        this.dialog.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);
    }

    @NonNull
    private View createView() {
        list = new RecyclerView(context);
        list.setLayoutManager(new LinearLayoutManager(context));
        itemTouchHelper = new ItemTouchHelper(simpleCallback);
        itemTouchHelper.attachToRecyclerView(list);
        adapter = new TabsAdapter(tabAdapterCallback);
        list.setAdapter(adapter);
        final Pair<List<Tab>, List<Tab>> navTabListPair = Utils.getNavTabList(context);
        tabsInPref = navTabListPair.first;
        // initially set newOrderTabs and newOtherTabs same as current tabs
        newOrderTabs = navTabListPair.first;
        newOtherTabs = navTabListPair.second;
        adapter.submitList(navTabListPair.first, navTabListPair.second);
        return list;
    }

    public interface Callback {
        void onSave(final boolean orderHasChanged);

        void onCancel();
    }
}
