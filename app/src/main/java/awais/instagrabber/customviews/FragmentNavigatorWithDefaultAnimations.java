package awais.instagrabber.customviews;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.FragmentManager;
import androidx.navigation.NavDestination;
import androidx.navigation.NavOptions;
import androidx.navigation.Navigator;
import androidx.navigation.fragment.FragmentNavigator;

import awais.instagrabber.R;

@Navigator.Name("fragment")
public class FragmentNavigatorWithDefaultAnimations extends FragmentNavigator {

    private final NavOptions emptyNavOptions = new NavOptions.Builder().build();
    // private final NavOptions defaultNavOptions = new NavOptions.Builder()
    //         .setEnterAnim(R.animator.nav_default_enter_anim)
    //         .setExitAnim(R.animator.nav_default_exit_anim)
    //         .setPopEnterAnim(R.animator.nav_default_pop_enter_anim)
    //         .setPopExitAnim(R.animator.nav_default_pop_exit_anim)
    //         .build();

    private final NavOptions defaultNavOptions = new NavOptions.Builder()
            .setEnterAnim(R.anim.slide_in_right)
            .setExitAnim(R.anim.slide_out_left)
            .setPopEnterAnim(android.R.anim.slide_in_left)
            .setPopExitAnim(android.R.anim.slide_out_right)
            .build();

    public FragmentNavigatorWithDefaultAnimations(@NonNull final Context context,
                                                  @NonNull final FragmentManager manager,
                                                  final int containerId) {
        super(context, manager, containerId);
    }

    @Nullable
    @Override
    public NavDestination navigate(@NonNull final Destination destination,
                                   @Nullable final Bundle args,
                                   @Nullable final NavOptions navOptions,
                                   @Nullable final Navigator.Extras navigatorExtras) {
        // this will try to fill in empty animations with defaults when no shared element transitions are set
        // https://developer.android.com/guide/navigation/navigation-animate-transitions#shared-element
        final boolean shouldUseTransitionsInstead = navigatorExtras != null;
        final NavOptions navOptions1 = shouldUseTransitionsInstead ? navOptions : fillEmptyAnimationsWithDefaults(navOptions);
        return super.navigate(destination, args, navOptions1, navigatorExtras);
    }

    private NavOptions fillEmptyAnimationsWithDefaults(@Nullable final NavOptions navOptions) {
        if (navOptions == null) {
            return defaultNavOptions;
        }
        return copyNavOptionsWithDefaultAnimations(navOptions);
    }

    @NonNull
    private NavOptions copyNavOptionsWithDefaultAnimations(@NonNull final NavOptions navOptions) {
        return new NavOptions.Builder()
                .setLaunchSingleTop(navOptions.shouldLaunchSingleTop())
                .setPopUpTo(navOptions.getPopUpTo(), navOptions.isPopUpToInclusive())
                .setEnterAnim(navOptions.getEnterAnim() == emptyNavOptions.getEnterAnim()
                              ? defaultNavOptions.getEnterAnim() : navOptions.getEnterAnim())
                .setExitAnim(navOptions.getExitAnim() == emptyNavOptions.getExitAnim()
                             ? defaultNavOptions.getExitAnim() : navOptions.getExitAnim())
                .setPopEnterAnim(navOptions.getPopEnterAnim() == emptyNavOptions.getPopEnterAnim()
                                 ? defaultNavOptions.getPopEnterAnim() : navOptions.getPopEnterAnim())
                .setPopExitAnim(navOptions.getPopExitAnim() == emptyNavOptions.getPopExitAnim()
                                ? defaultNavOptions.getPopExitAnim() : navOptions.getPopExitAnim())
                .build();
    }
}
