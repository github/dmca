package thoughtbot.expandableadapter;

import androidx.annotation.NonNull;

import java.util.ArrayList;

public final class ExpandableList {
    private final int groupsSize;
    public final ArrayList<ExpandableGroup> groups;
    public final boolean[] expandedGroupIndexes;

    public ExpandableList(@NonNull final ArrayList<ExpandableGroup> groups) {
        this.groups = groups;
        this.groupsSize = groups.size();
        this.expandedGroupIndexes = new boolean[groupsSize];
    }

    public int getVisibleItemCount() {
        int count = 0;
        for (int i = 0; i < groupsSize; i++) count = count + numberOfVisibleItemsInGroup(i);
        return count;
    }

    @NonNull
    public ExpandableListPosition getUnflattenedPosition(final int flPos) {
        int adapted = flPos;
        for (int i = 0; i < groupsSize; i++) {
            final int groupItemCount = numberOfVisibleItemsInGroup(i);
            if (adapted == 0)
                return ExpandableListPosition.obtain(ExpandableListPosition.GROUP, i, -1, flPos);
            else if (adapted < groupItemCount)
                return ExpandableListPosition.obtain(ExpandableListPosition.CHILD, i, adapted - 1, flPos);
            adapted = adapted - groupItemCount;
        }
        throw new RuntimeException("Unknown state");
    }

    private int numberOfVisibleItemsInGroup(final int group) {
        return expandedGroupIndexes[group] ? groups.get(group).getItemCount(true) + 1 : 1;
    }

    public int getFlattenedGroupIndex(@NonNull final ExpandableListPosition listPosition) {
        int runningTotal = 0;
        for (int i = 0; i < listPosition.groupPos; i++) runningTotal = runningTotal + numberOfVisibleItemsInGroup(i);
        return runningTotal;
    }

    public ExpandableGroup getExpandableGroup(@NonNull ExpandableListPosition listPosition) {
        return groups.get(listPosition.groupPos);
    }
}