package awais.instagrabber.adapters;

import androidx.annotation.NonNull;
import androidx.fragment.app.Fragment;
import androidx.viewpager2.adapter.FragmentStateAdapter;

import java.util.List;

import awais.instagrabber.fragments.search.SearchCategoryFragment;
import awais.instagrabber.models.enums.FavoriteType;

public class SearchCategoryAdapter extends FragmentStateAdapter {

    private final List<FavoriteType> categories;

    public SearchCategoryAdapter(@NonNull final Fragment fragment,
                                 @NonNull final List<FavoriteType> categories) {
        super(fragment);
        this.categories = categories;

    }

    @NonNull
    @Override
    public Fragment createFragment(final int position) {
        return SearchCategoryFragment.newInstance(categories.get(position));
    }

    @Override
    public int getItemCount() {
        return categories.size();
    }
}
