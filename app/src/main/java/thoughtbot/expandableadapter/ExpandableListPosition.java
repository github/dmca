package thoughtbot.expandableadapter;

import androidx.annotation.NonNull;

public class ExpandableListPosition {
    private static final ExpandableListPosition LIST_POSITION = new ExpandableListPosition();
    public final static int CHILD = 1;
    public final static int GROUP = 2;
    private int flatListPos;
    public int groupPos;
    public int childPos;
    public int type;

    @NonNull
    public static ExpandableListPosition obtain(final int type, final int groupPos, final int childPos, final int flatListPos) {
        LIST_POSITION.type = type;
        LIST_POSITION.groupPos = groupPos;
        LIST_POSITION.childPos = childPos;
        LIST_POSITION.flatListPos = flatListPos;
        return LIST_POSITION;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        //if (o != null && getClass() == o.getClass()) {
        if (o instanceof ExpandableListPosition) {
            final ExpandableListPosition that = (ExpandableListPosition) o;
            if (groupPos != that.groupPos) return false;
            if (childPos != that.childPos) return false;
            if (flatListPos != that.flatListPos) return false;
            return type == that.type;
        }
        return false;
    }

    @Override
    public int hashCode() {
        return 31 * (31 * (31 * groupPos + childPos) + flatListPos) + type;
    }
}