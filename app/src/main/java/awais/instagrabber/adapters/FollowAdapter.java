package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Filter;
import android.widget.Filterable;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.viewholder.FollowsViewHolder;
import awais.instagrabber.databinding.ItemFollowBinding;
import awais.instagrabber.interfaces.OnGroupClickListener;
import awais.instagrabber.models.FollowModel;
import awais.instagrabber.utils.TextUtils;
import thoughtbot.expandableadapter.ExpandableGroup;
import thoughtbot.expandableadapter.ExpandableList;
import thoughtbot.expandableadapter.ExpandableListPosition;
import thoughtbot.expandableadapter.GroupViewHolder;

// thanks to ThoughtBot's ExpandableRecyclerViewAdapter
//   https://github.com/thoughtbot/expandable-recycler-view
public final class FollowAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> implements OnGroupClickListener, Filterable {
    private final Filter filter = new Filter() {
        @Nullable
        @Override
        protected FilterResults performFiltering(final CharSequence filter) {
            if (expandableList.groups != null) {
                final boolean isFilterEmpty = TextUtils.isEmpty(filter);
                final String query = isFilterEmpty ? null : filter.toString().toLowerCase();

                for (int x = 0; x < expandableList.groups.size(); ++x) {
                    final ExpandableGroup expandableGroup = expandableList.groups.get(x);
                    final List<FollowModel> items = expandableGroup.getItems(false);
                    final int itemCount = expandableGroup.getItemCount(false);

                    for (int i = 0; i < itemCount; ++i) {
                        final FollowModel followModel = items.get(i);

                        if (isFilterEmpty) followModel.setShown(true);
                        else followModel.setShown(hasKey(query, followModel.getUsername(), followModel.getFullName()));
                    }
                }
            }
            return null;
        }

        private boolean hasKey(final String key, final String username, final String name) {
            if (TextUtils.isEmpty(key)) return true;
            final boolean hasUserName = username != null && username.toLowerCase().contains(key);
            if (!hasUserName && name != null) return name.toLowerCase().contains(key);
            return true;
        }

        @Override
        protected void publishResults(final CharSequence constraint, final FilterResults results) {
            notifyDataSetChanged();
        }
    };
    private final View.OnClickListener onClickListener;
    private final ExpandableList expandableList;
    private final boolean hasManyGroups;

    public FollowAdapter(final View.OnClickListener onClickListener, @NonNull final ArrayList<ExpandableGroup> groups) {
        this.expandableList = new ExpandableList(groups);
        this.onClickListener = onClickListener;
        this.hasManyGroups = groups.size() > 1;
    }

    @Override
    public Filter getFilter() {
        return filter;
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final boolean isGroup = hasManyGroups && viewType == ExpandableListPosition.GROUP;
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final View view;
        if (isGroup) {
            view = layoutInflater.inflate(R.layout.header_follow, parent, false);
            return new GroupViewHolder(view, this);
        } else {
            final ItemFollowBinding binding = ItemFollowBinding.inflate(layoutInflater, parent, false);
            return new FollowsViewHolder(binding);
        }
    }

    @Override
    public void onBindViewHolder(@NonNull final RecyclerView.ViewHolder holder, final int position) {
        final ExpandableListPosition listPos = expandableList.getUnflattenedPosition(position);
        final ExpandableGroup group = expandableList.getExpandableGroup(listPos);

        if (hasManyGroups && listPos.type == ExpandableListPosition.GROUP) {
            final GroupViewHolder gvh = (GroupViewHolder) holder;
            gvh.setTitle(group.getTitle());
            gvh.toggle(isGroupExpanded(group));
            return;
        }
        final FollowModel model = group.getItems(true).get(hasManyGroups ? listPos.childPos : position);
        ((FollowsViewHolder) holder).bind(model, onClickListener);
    }

    @Override
    public int getItemCount() {
        return expandableList.getVisibleItemCount() - (hasManyGroups ? 0 : 1);
    }

    @Override
    public int getItemViewType(final int position) {
        return !hasManyGroups ? 0 : expandableList.getUnflattenedPosition(position).type;
    }

    @Override
    public void toggleGroup(final int flatPos) {
        final ExpandableListPosition listPosition = expandableList.getUnflattenedPosition(flatPos);

        final int groupPos = listPosition.groupPos;
        final int positionStart = expandableList.getFlattenedGroupIndex(listPosition) + 1;
        final int positionEnd = expandableList.groups.get(groupPos).getItemCount(true);

        final boolean isExpanded = expandableList.expandedGroupIndexes[groupPos];
        expandableList.expandedGroupIndexes[groupPos] = !isExpanded;
        notifyItemChanged(positionStart - 1);
        if (positionEnd > 0) {
            if (isExpanded) notifyItemRangeRemoved(positionStart, positionEnd);
            else notifyItemRangeInserted(positionStart, positionEnd);
        }
    }

    public boolean isGroupExpanded(final ExpandableGroup group) {
        return expandableList.expandedGroupIndexes[expandableList.groups.indexOf(group)];
    }
}