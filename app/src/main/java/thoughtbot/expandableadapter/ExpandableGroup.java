package thoughtbot.expandableadapter;

import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.models.FollowModel;

public class ExpandableGroup {
    private final String title;
    private final List<FollowModel> items;

    public ExpandableGroup(final String title, final List<FollowModel> items) {
        this.title = title;
        this.items = items;
    }

    public String getTitle() {
        return title;
    }

    public List<FollowModel> getItems(final boolean filtered) {
        if (!filtered) return items;
        final ArrayList<FollowModel> followModels = new ArrayList<>();
        for (final FollowModel followModel : items) if (followModel.isShown()) followModels.add(followModel);
        return followModels;
    }

    public int getItemCount(final boolean filtered) {
        if (items != null) {
            final int size = items.size();
            if (filtered) {
                int finalSize = 0;
                for (int i = 0; i < size; ++i) if (items.get(i).isShown()) ++finalSize;
                return finalSize;
            }
            return size;
        }
        return 0;
    }
}