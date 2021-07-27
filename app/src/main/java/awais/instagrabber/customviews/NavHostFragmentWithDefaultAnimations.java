package awais.instagrabber.customviews;

import android.os.Bundle;

import androidx.annotation.NavigationRes;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.navigation.NavController;
import androidx.navigation.Navigator;
import androidx.navigation.fragment.FragmentNavigator;
import androidx.navigation.fragment.NavHostFragment;

public class NavHostFragmentWithDefaultAnimations extends NavHostFragment {
    private static final String KEY_GRAPH_ID = "android-support-nav:fragment:graphId";
    private static final String KEY_START_DESTINATION_ARGS =
            "android-support-nav:fragment:startDestinationArgs";
    private static final String KEY_NAV_CONTROLLER_STATE =
            "android-support-nav:fragment:navControllerState";
    private static final String KEY_DEFAULT_NAV_HOST = "android-support-nav:fragment:defaultHost";

    @NonNull
    public static NavHostFragment create(@NavigationRes int graphResId) {
        return create(graphResId, null);
    }

    @NonNull
    public static NavHostFragment create(@NavigationRes int graphResId,
                                         @Nullable Bundle startDestinationArgs) {
        Bundle b = null;
        if (graphResId != 0) {
            b = new Bundle();
            b.putInt(KEY_GRAPH_ID, graphResId);
        }
        if (startDestinationArgs != null) {
            if (b == null) {
                b = new Bundle();
            }
            b.putBundle(KEY_START_DESTINATION_ARGS, startDestinationArgs);
        }

        final NavHostFragmentWithDefaultAnimations result = new NavHostFragmentWithDefaultAnimations();
        if (b != null) {
            result.setArguments(b);
        }
        return result;
    }

    @NonNull
    @Override
    protected Navigator<? extends FragmentNavigator.Destination> createFragmentNavigator() {
        return new FragmentNavigatorWithDefaultAnimations(requireContext(), getChildFragmentManager(), getId());
    }

    @Override
    protected void onCreateNavController(@NonNull final NavController navController) {
        super.onCreateNavController(navController);
        navController.getNavigatorProvider()
                     .addNavigator(new FragmentNavigatorWithDefaultAnimations(requireContext(), getChildFragmentManager(), getId()));
    }
}
