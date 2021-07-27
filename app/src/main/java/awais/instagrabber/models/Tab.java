package awais.instagrabber.models;

import androidx.annotation.DrawableRes;
import androidx.annotation.IdRes;
import androidx.annotation.NavigationRes;
import androidx.annotation.NonNull;

import java.util.Objects;

public class Tab {
    private final int iconResId;
    private final String title;
    private final boolean removable;

    /**
     * This is name part of the navigation resource
     * eg: @navigation/<b>graphName</b>
     */
    private final String graphName;

    /**
     * This is the actual resource id of the navigation resource (R.navigation.graphName = navigationResId)
     */
    private final int navigationResId;

    /**
     * This is the resource id of the root navigation tag of the navigation resource.
     * <p>eg: inside R.navigation.direct_messages_nav_graph, the id of the root tag is R.id.direct_messages_nav_graph.
     * <p>So this field would equal to the value of R.id.direct_messages_nav_graph
     */
    private final int navigationRootId;

    /**
     * This is the start destination of the nav graph
     */
    private final int startDestinationFragmentId;

    public Tab(@DrawableRes final int iconResId,
               @NonNull final String title,
               final boolean removable,
               @NonNull final String graphName,
               @NavigationRes final int navigationResId,
               @IdRes final int navigationRootId,
               @IdRes final int startDestinationFragmentId) {
        this.iconResId = iconResId;
        this.title = title;
        this.removable = removable;
        this.graphName = graphName;
        this.navigationResId = navigationResId;
        this.navigationRootId = navigationRootId;
        this.startDestinationFragmentId = startDestinationFragmentId;
    }

    public int getIconResId() {
        return iconResId;
    }

    public String getTitle() {
        return title;
    }

    public boolean isRemovable() {
        return removable;
    }

    public String getGraphName() {
        return graphName;
    }

    public int getNavigationResId() {
        return navigationResId;
    }

    public int getNavigationRootId() {
        return navigationRootId;
    }

    public int getStartDestinationFragmentId() {
        return startDestinationFragmentId;
    }

    @Override
    public boolean equals(final Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        final Tab tab = (Tab) o;
        return iconResId == tab.iconResId &&
                removable == tab.removable &&
                navigationResId == tab.navigationResId &&
                navigationRootId == tab.navigationRootId &&
                startDestinationFragmentId == tab.startDestinationFragmentId &&
                Objects.equals(title, tab.title) &&
                Objects.equals(graphName, tab.graphName);
    }

    @Override
    public int hashCode() {
        return Objects.hash(iconResId, title, removable, graphName, navigationResId, navigationRootId, startDestinationFragmentId);
    }

    @NonNull
    @Override
    public String toString() {
        return "Tab{" +
                "title='" + title + '\'' +
                ", removable=" + removable +
                ", graphName='" + graphName + '\'' +
                '}';
    }
}
