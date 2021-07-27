package awais.instagrabber.activities;

import android.animation.LayoutTransition;
import android.annotation.SuppressLint;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.text.Editable;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.IdRes;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.widget.Toolbar;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.core.app.NotificationManagerCompat;
import androidx.core.provider.FontRequest;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.emoji.text.EmojiCompat;
import androidx.emoji.text.FontRequestEmojiCompatConfig;
import androidx.fragment.app.FragmentManager;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.Observer;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavBackStackEntry;
import androidx.navigation.NavController;
import androidx.navigation.NavDestination;
import androidx.navigation.ui.NavigationUI;

import com.google.android.material.appbar.AppBarLayout;
import com.google.android.material.appbar.CollapsingToolbarLayout;
import com.google.android.material.badge.BadgeDrawable;
import com.google.android.material.behavior.HideBottomViewOnScrollBehavior;
import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.textfield.TextInputLayout;
import com.google.common.collect.ImmutableList;
import com.google.common.collect.Iterators;

import java.util.Collections;
import java.util.Deque;
import java.util.List;
import java.util.UUID;
import java.util.stream.Collectors;

import awais.instagrabber.BuildConfig;
import awais.instagrabber.R;
import awais.instagrabber.customviews.emoji.EmojiVariantManager;
import awais.instagrabber.customviews.helpers.RootViewDeferringInsetsCallback;
import awais.instagrabber.customviews.helpers.TextWatcherAdapter;
import awais.instagrabber.databinding.ActivityMainBinding;
import awais.instagrabber.fragments.PostViewV2Fragment;
import awais.instagrabber.fragments.directmessages.DirectMessageInboxFragmentDirections;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.models.IntentModel;
import awais.instagrabber.models.Tab;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.services.ActivityCheckerService;
import awais.instagrabber.services.DMSyncAlarmReceiver;
import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.FlavorTown;
import awais.instagrabber.utils.IntentUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.utils.emoji.EmojiParser;
import awais.instagrabber.viewmodels.AppStateViewModel;
import awais.instagrabber.viewmodels.DirectInboxViewModel;
import awais.instagrabber.webservices.GraphQLService;
import awais.instagrabber.webservices.MediaService;
import awais.instagrabber.webservices.RetrofitFactory;
import awais.instagrabber.webservices.ServiceCallback;

import static awais.instagrabber.utils.NavigationExtensions.setupWithNavController;
import static awais.instagrabber.utils.Utils.settingsHelper;

public class MainActivity extends BaseLanguageActivity implements FragmentManager.OnBackStackChangedListener {
    private static final String TAG = "MainActivity";
    private static final String FIRST_FRAGMENT_GRAPH_INDEX_KEY = "firstFragmentGraphIndex";
    private static final String LAST_SELECT_NAV_MENU_ID = "lastSelectedNavMenuId";
    private static final List<Integer> SEARCH_VISIBLE_DESTINATIONS = ImmutableList.of(
            R.id.feedFragment,
            R.id.profileFragment,
            R.id.directMessagesInboxFragment,
            R.id.discoverFragment,
            R.id.favoritesFragment,
            R.id.hashTagFragment,
            R.id.locationFragment
    );

    private static MainActivity instance;

    private ActivityMainBinding binding;
    private LiveData<NavController> currentNavControllerLiveData;
    private MenuItem searchMenuItem;
    private int firstFragmentGraphIndex;
    private int lastSelectedNavMenuId;
    private boolean isActivityCheckerServiceBound = false;
    private boolean isBackStackEmpty = false;
    private boolean isLoggedIn;
    private HideBottomViewOnScrollBehavior<BottomNavigationView> behavior;
    private List<Tab> currentTabs;
    private List<Integer> showBottomViewDestinations = Collections.emptyList();
    private GraphQLService graphQLService = null;
    private MediaService mediaService = null;

    private final ServiceConnection serviceConnection = new ServiceConnection() {
        @Override
        public void onServiceConnected(final ComponentName name, final IBinder service) {
            // final ActivityCheckerService.LocalBinder binder = (ActivityCheckerService.LocalBinder) service;
            // final ActivityCheckerService activityCheckerService = binder.getService();
            isActivityCheckerServiceBound = true;
        }

        @Override
        public void onServiceDisconnected(final ComponentName name) {
            isActivityCheckerServiceBound = false;
        }
    };

    public static MainActivity getInstance() {
        return instance;
    }

    @Override
    protected void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        instance = this;
        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setupCookie();
        if (settingsHelper.getBoolean(PreferenceKeys.FLAG_SECURE)) {
            getWindow().setFlags(WindowManager.LayoutParams.FLAG_SECURE, WindowManager.LayoutParams.FLAG_SECURE);
        }
        setContentView(binding.getRoot());
        final Toolbar toolbar = binding.toolbar;
        setSupportActionBar(toolbar);
        final RootViewDeferringInsetsCallback deferringInsetsCallback = new RootViewDeferringInsetsCallback(
                WindowInsetsCompat.Type.systemBars(),
                WindowInsetsCompat.Type.ime()
        );
        ViewCompat.setWindowInsetsAnimationCallback(binding.getRoot(), deferringInsetsCallback);
        ViewCompat.setOnApplyWindowInsetsListener(binding.getRoot(), deferringInsetsCallback);
        WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
        createNotificationChannels();
        try {
            final CoordinatorLayout.LayoutParams layoutParams = (CoordinatorLayout.LayoutParams) binding.bottomNavView.getLayoutParams();
            //noinspection unchecked
            behavior = (HideBottomViewOnScrollBehavior<BottomNavigationView>) layoutParams.getBehavior();
        } catch (Exception e) {
            Log.e(TAG, "onCreate: ", e);
        }
        if (savedInstanceState == null) {
            setupBottomNavigationBar(true);
        }
        if (!BuildConfig.isPre) {
            final boolean checkUpdates = settingsHelper.getBoolean(PreferenceKeys.CHECK_UPDATES);
            if (checkUpdates) FlavorTown.updateCheck(this);
        }
        FlavorTown.changelogCheck(this);
        new ViewModelProvider(this).get(AppStateViewModel.class); // Just initiate the App state here
        final Intent intent = getIntent();
        handleIntent(intent);
        if (isLoggedIn && settingsHelper.getBoolean(PreferenceKeys.CHECK_ACTIVITY)) {
            bindActivityCheckerService();
        }
        getSupportFragmentManager().addOnBackStackChangedListener(this);
        // Initialise the internal map
        AppExecutors.getInstance().tasksThread().execute(() -> {
            EmojiParser.setup(this);
            EmojiVariantManager.getInstance();
        });
        initEmojiCompat();
        // initDmService();
        initDmUnreadCount();
        initSearchInput();
    }

    private void setupCookie() {
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        long userId = 0;
        String csrfToken = null;
        if (!TextUtils.isEmpty(cookie)) {
            userId = CookieUtils.getUserIdFromCookie(cookie);
            csrfToken = CookieUtils.getCsrfTokenFromCookie(cookie);
        }
        if (TextUtils.isEmpty(cookie) || userId == 0 || TextUtils.isEmpty(csrfToken)) {
            isLoggedIn = false;
            return;
        }
        final String deviceUuid = settingsHelper.getString(Constants.DEVICE_UUID);
        if (TextUtils.isEmpty(deviceUuid)) {
            settingsHelper.putString(Constants.DEVICE_UUID, UUID.randomUUID().toString());
        }
        CookieUtils.setupCookies(cookie);
        isLoggedIn = true;
    }

    private void initDmService() {
        if (!isLoggedIn) return;
        final boolean enabled = settingsHelper.getBoolean(PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH);
        if (!enabled) return;
        DMSyncAlarmReceiver.setAlarm(this);
    }

    private void initDmUnreadCount() {
        if (!isLoggedIn) return;
        final DirectInboxViewModel directInboxViewModel = new ViewModelProvider(this).get(DirectInboxViewModel.class);
        directInboxViewModel.getUnseenCount().observe(this, unseenCountResource -> {
            if (unseenCountResource == null) return;
            final Integer unseenCount = unseenCountResource.data;
            setNavBarDMUnreadCountBadge(unseenCount == null ? 0 : unseenCount);
        });
    }

    private void initSearchInput() {
        binding.searchInputLayout.setEndIconOnClickListener(v -> {
            final EditText editText = binding.searchInputLayout.getEditText();
            if (editText == null) return;
            editText.setText("");
        });
        binding.searchInputLayout.addOnEditTextAttachedListener(textInputLayout -> {
            textInputLayout.setEndIconVisible(false);
            final EditText editText = textInputLayout.getEditText();
            if (editText == null) return;
            editText.addTextChangedListener(new TextWatcherAdapter() {
                @Override
                public void afterTextChanged(final Editable s) {
                    binding.searchInputLayout.setEndIconVisible(!TextUtils.isEmpty(s));
                }
            });
        });
    }

    @Override
    public boolean onCreateOptionsMenu(final Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        searchMenuItem = menu.findItem(R.id.search);
        final NavController navController = currentNavControllerLiveData.getValue();
        if (navController != null) {
            final NavDestination currentDestination = navController.getCurrentDestination();
            if (currentDestination != null) {
                @SuppressLint("RestrictedApi") final Deque<NavBackStackEntry> backStack = navController.getBackStack();
                setupMenu(backStack.size(), currentDestination.getId());
            }
        }
        // if (binding.searchInputLayout.getVisibility() == View.VISIBLE) {
        //     searchMenuItem.setVisible(false).setEnabled(false);
        //     return true;
        // }
        // searchMenuItem.setVisible(true).setEnabled(true);
        // if (showSearch && currentNavControllerLiveData != null) {
        //     final NavController navController = currentNavControllerLiveData.getValue();
        //     if (navController != null) {
        //         final NavDestination currentDestination = navController.getCurrentDestination();
        //         if (currentDestination != null) {
        //             final int destinationId = currentDestination.getId();
        //             showSearch = destinationId == R.id.profileFragment;
        //         }
        //     }
        // }
        // if (!showSearch) {
        //     searchMenuItem.setVisible(false);
        //     return true;
        // }
        // return setupSearchView();
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull final MenuItem item) {
        if (item.getItemId() == R.id.search) {
            final NavController navController = currentNavControllerLiveData.getValue();
            if (navController == null) return false;
            try {
                navController.navigate(R.id.action_global_search);
                return true;
            } catch (Exception e) {
                Log.e(TAG, "onOptionsItemSelected: ", e);
            }
            return false;
        }
        return super.onOptionsItemSelected(item);
    }

    @Override
    protected void onSaveInstanceState(@NonNull final Bundle outState) {
        outState.putString(FIRST_FRAGMENT_GRAPH_INDEX_KEY, String.valueOf(firstFragmentGraphIndex));
        outState.putString(LAST_SELECT_NAV_MENU_ID, String.valueOf(binding.bottomNavView.getSelectedItemId()));
        super.onSaveInstanceState(outState);
    }

    @Override
    protected void onRestoreInstanceState(@NonNull final Bundle savedInstanceState) {
        super.onRestoreInstanceState(savedInstanceState);
        final String key = (String) savedInstanceState.get(FIRST_FRAGMENT_GRAPH_INDEX_KEY);
        if (key != null) {
            try {
                firstFragmentGraphIndex = Integer.parseInt(key);
            } catch (NumberFormatException ignored) { }
        }
        final String lastSelected = (String) savedInstanceState.get(LAST_SELECT_NAV_MENU_ID);
        if (lastSelected != null) {
            try {
                lastSelectedNavMenuId = Integer.parseInt(lastSelected);
            } catch (NumberFormatException ignored) { }
        }
        setupBottomNavigationBar(false);
    }

    @Override
    public boolean onSupportNavigateUp() {
        if (currentNavControllerLiveData == null) return false;
        final NavController navController = currentNavControllerLiveData.getValue();
        if (navController == null) return false;
        return navController.navigateUp();
    }

    @Override
    protected void onNewIntent(final Intent intent) {
        super.onNewIntent(intent);
        handleIntent(intent);
    }

    @Override
    protected void onDestroy() {
        try {
            super.onDestroy();
        } catch (Exception e) {
            Log.e(TAG, "onDestroy: ", e);
        }
        unbindActivityCheckerService();
        try {
            RetrofitFactory.getInstance().destroy();
        } catch (Exception e) {
            Log.e(TAG, "onDestroy: ", e);
        }
        instance = null;
    }

    @Override
    public void onBackPressed() {
        int currentNavControllerBackStack = 2;
        if (currentNavControllerLiveData != null) {
            final NavController navController = currentNavControllerLiveData.getValue();
            if (navController != null) {
                @SuppressLint("RestrictedApi") final Deque<NavBackStackEntry> backStack = navController.getBackStack();
                currentNavControllerBackStack = backStack.size();
            }
        }
        if (isTaskRoot() && isBackStackEmpty && currentNavControllerBackStack == 2) {
            finishAfterTransition();
            return;
        }
        if (!isFinishing()) {
            try {
                super.onBackPressed();
            } catch (Exception e) {
                Log.e(TAG, "onBackPressed: ", e);
                finish();
            }
        }
    }

    @Override
    public void onBackStackChanged() {
        final int backStackEntryCount = getSupportFragmentManager().getBackStackEntryCount();
        isBackStackEmpty = backStackEntryCount == 0;
    }

    private void createNotificationChannels() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) return;
        final NotificationManagerCompat notificationManager = NotificationManagerCompat.from(getApplicationContext());
        notificationManager.createNotificationChannel(new NotificationChannel(Constants.DOWNLOAD_CHANNEL_ID,
                                                                              Constants.DOWNLOAD_CHANNEL_NAME,
                                                                              NotificationManager.IMPORTANCE_DEFAULT));
        notificationManager.createNotificationChannel(new NotificationChannel(Constants.ACTIVITY_CHANNEL_ID,
                                                                              Constants.ACTIVITY_CHANNEL_NAME,
                                                                              NotificationManager.IMPORTANCE_DEFAULT));
        notificationManager.createNotificationChannel(new NotificationChannel(Constants.DM_UNREAD_CHANNEL_ID,
                                                                              Constants.DM_UNREAD_CHANNEL_NAME,
                                                                              NotificationManager.IMPORTANCE_DEFAULT));
        final NotificationChannel silentNotificationChannel = new NotificationChannel(Constants.SILENT_NOTIFICATIONS_CHANNEL_ID,
                                                                                      Constants.SILENT_NOTIFICATIONS_CHANNEL_NAME,
                                                                                      NotificationManager.IMPORTANCE_LOW);
        silentNotificationChannel.setSound(null, null);
        notificationManager.createNotificationChannel(silentNotificationChannel);
    }

    private void setupBottomNavigationBar(final boolean setDefaultTabFromSettings) {
        currentTabs = !isLoggedIn ? setupAnonBottomNav() : setupMainBottomNav();
        final List<Integer> mainNavList = currentTabs.stream()
                                                     .map(Tab::getNavigationResId)
                                                     .collect(Collectors.toList());
        showBottomViewDestinations = currentTabs.stream()
                                                .map(Tab::getStartDestinationFragmentId)
                                                .collect(Collectors.toList());
        showBottomViewDestinations.add(R.id.postViewFragment);
        if (setDefaultTabFromSettings) {
            setSelectedTab(currentTabs);
        } else {
            binding.bottomNavView.setSelectedItemId(lastSelectedNavMenuId);
        }
        final LiveData<NavController> navControllerLiveData = setupWithNavController(
                binding.bottomNavView,
                mainNavList,
                getSupportFragmentManager(),
                R.id.main_nav_host,
                getIntent(),
                firstFragmentGraphIndex);
        navControllerLiveData.observe(this, navController -> setupNavigation(binding.toolbar, navController));
        currentNavControllerLiveData = navControllerLiveData;
    }

    private void setSelectedTab(final List<Tab> tabs) {
        final String defaultTabResNameString = settingsHelper.getString(Constants.DEFAULT_TAB);
        try {
            int navId = 0;
            if (!TextUtils.isEmpty(defaultTabResNameString)) {
                navId = getResources().getIdentifier(defaultTabResNameString, "navigation", getPackageName());
            }
            final int navGraph = isLoggedIn ? R.navigation.feed_nav_graph
                                            : R.navigation.profile_nav_graph;
            final int defaultNavId = navId <= 0 ? navGraph : navId;
            int index = Iterators.indexOf(tabs.iterator(), tab -> {
                if (tab == null) return false;
                return tab.getNavigationResId() == defaultNavId;
            });
            if (index < 0 || index >= tabs.size()) index = 0;
            firstFragmentGraphIndex = index;
            setBottomNavSelectedTab(tabs.get(index));
        } catch (Exception e) {
            Log.e(TAG, "Error parsing id", e);
        }
    }

    private List<Tab> setupAnonBottomNav() {
        final int selectedItemId = binding.bottomNavView.getSelectedItemId();
        final Tab favoriteTab = new Tab(R.drawable.ic_star_24,
                                        getString(R.string.title_favorites),
                                        false,
                                        "favorites_nav_graph",
                                        R.navigation.favorites_nav_graph,
                                        R.id.favorites_nav_graph,
                                        R.id.favoritesFragment);
        final Tab profileTab = new Tab(R.drawable.ic_person_24,
                                       getString(R.string.profile),
                                       false,
                                       "profile_nav_graph",
                                       R.navigation.profile_nav_graph,
                                       R.id.profile_nav_graph,
                                       R.id.profileFragment);
        final Tab moreTab = new Tab(R.drawable.ic_more_horiz_24,
                                    getString(R.string.more),
                                    false,
                                    "more_nav_graph",
                                    R.navigation.more_nav_graph,
                                    R.id.more_nav_graph,
                                    R.id.morePreferencesFragment);
        final Menu menu = binding.bottomNavView.getMenu();
        menu.clear();
        menu.add(0, favoriteTab.getNavigationRootId(), 0, favoriteTab.getTitle()).setIcon(favoriteTab.getIconResId());
        menu.add(0, profileTab.getNavigationRootId(), 0, profileTab.getTitle()).setIcon(profileTab.getIconResId());
        menu.add(0, moreTab.getNavigationRootId(), 0, moreTab.getTitle()).setIcon(moreTab.getIconResId());
        if (selectedItemId != R.id.profile_nav_graph
                && selectedItemId != R.id.more_nav_graph
                && selectedItemId != R.id.favorites_nav_graph) {
            setBottomNavSelectedTab(profileTab);
        }
        return ImmutableList.of(favoriteTab, profileTab, moreTab);
    }

    private List<Tab> setupMainBottomNav() {
        final Menu menu = binding.bottomNavView.getMenu();
        menu.clear();
        final List<Tab> navTabList = Utils.getNavTabList(this).first;
        for (final Tab tab : navTabList) {
            menu.add(0, tab.getNavigationRootId(), 0, tab.getTitle()).setIcon(tab.getIconResId());
        }
        return navTabList;
    }

    private void setBottomNavSelectedTab(@NonNull final Tab tab) {
        binding.bottomNavView.setSelectedItemId(tab.getNavigationRootId());
    }

    private void setBottomNavSelectedTab(@SuppressWarnings("SameParameterValue") @IdRes final int navGraphRootId) {
        binding.bottomNavView.setSelectedItemId(navGraphRootId);
    }

    private void setupNavigation(final Toolbar toolbar, final NavController navController) {
        if (navController == null) return;
        NavigationUI.setupWithNavController(toolbar, navController);
        navController.addOnDestinationChangedListener((controller, destination, arguments) -> {
            if (destination.getId() == R.id.directMessagesThreadFragment && arguments != null) {
                // Set the thread title earlier for better ux
                final String title = arguments.getString("title");
                final ActionBar actionBar = getSupportActionBar();
                if (actionBar != null && !TextUtils.isEmpty(title)) {
                    actionBar.setTitle(title);
                }
            }
            // below is a hack to check if we are at the end of the current stack, to setup the search view
            binding.appBarLayout.setExpanded(true, true);
            final int destinationId = destination.getId();
            @SuppressLint("RestrictedApi") final Deque<NavBackStackEntry> backStack = navController.getBackStack();
            setupMenu(backStack.size(), destinationId);
            final boolean contains = showBottomViewDestinations.contains(destinationId);
            binding.getRoot().post(() -> {
                binding.bottomNavView.setVisibility(contains ? View.VISIBLE : View.GONE);
                if (contains && behavior != null) {
                    behavior.slideUp(binding.bottomNavView);
                }
            });
            // explicitly hide keyboard when we navigate
            final View view = getCurrentFocus();
            Utils.hideKeyboard(view);
        });
    }

    private void setupMenu(final int backStackSize, final int destinationId) {
        if (searchMenuItem == null) return;
        if (backStackSize >= 2 && SEARCH_VISIBLE_DESTINATIONS.contains(destinationId)) {
            searchMenuItem.setVisible(true);
            return;
        }
        searchMenuItem.setVisible(false);
    }

    private void setScrollingBehaviour() {
        final CoordinatorLayout.LayoutParams layoutParams = (CoordinatorLayout.LayoutParams) binding.mainNavHost.getLayoutParams();
        layoutParams.setBehavior(new AppBarLayout.ScrollingViewBehavior());
        binding.mainNavHost.requestLayout();
    }

    private void removeScrollingBehaviour() {
        final CoordinatorLayout.LayoutParams layoutParams = (CoordinatorLayout.LayoutParams) binding.mainNavHost.getLayoutParams();
        layoutParams.setBehavior(null);
        binding.mainNavHost.requestLayout();
    }

    private void handleIntent(final Intent intent) {
        if (intent == null) return;
        final String action = intent.getAction();
        final String type = intent.getType();
        // Log.d(TAG, action + " " + type);
        if (Intent.ACTION_MAIN.equals(action)) return;
        if (Constants.ACTION_SHOW_ACTIVITY.equals(action)) {
            showActivityView();
            return;
        }
        if (Constants.ACTION_SHOW_DM_THREAD.equals(action)) {
            showThread(intent);
            return;
        }
        if (Intent.ACTION_SEND.equals(action) && type != null) {
            if (type.equals("text/plain")) {
                handleUrl(intent.getStringExtra(Intent.EXTRA_TEXT));
            }
            return;
        }
        if (Intent.ACTION_VIEW.equals(action)) {
            final Uri data = intent.getData();
            if (data == null) return;
            handleUrl(data.toString());
        }
    }

    private void showThread(@NonNull final Intent intent) {
        final String threadId = intent.getStringExtra(Constants.DM_THREAD_ACTION_EXTRA_THREAD_ID);
        final String threadTitle = intent.getStringExtra(Constants.DM_THREAD_ACTION_EXTRA_THREAD_TITLE);
        navigateToThread(threadId, threadTitle);
    }

    public void navigateToThread(final String threadId, final String threadTitle) {
        if (threadId == null || threadTitle == null) return;
        currentNavControllerLiveData.observe(this, new Observer<NavController>() {
            @Override
            public void onChanged(final NavController navController) {
                if (navController == null) return;
                if (navController.getGraph().getId() != R.id.direct_messages_nav_graph) return;
                try {
                    final NavDestination currentDestination = navController.getCurrentDestination();
                    if (currentDestination != null && currentDestination.getId() == R.id.directMessagesInboxFragment) {
                        // if we are already on the inbox page, navigate to the thread
                        // need handler.post() to wait for the fragment manager to be ready to navigate
                        new Handler().post(() -> {
                            final DirectMessageInboxFragmentDirections.ActionInboxToThread action = DirectMessageInboxFragmentDirections
                                    .actionInboxToThread(threadId, threadTitle);
                            navController.navigate(action);
                        });
                        return;
                    }
                    // add a destination change listener to navigate to thread once we are on the inbox page
                    navController.addOnDestinationChangedListener(new NavController.OnDestinationChangedListener() {
                        @Override
                        public void onDestinationChanged(@NonNull final NavController controller,
                                                         @NonNull final NavDestination destination,
                                                         @Nullable final Bundle arguments) {
                            if (destination.getId() == R.id.directMessagesInboxFragment) {
                                final DirectMessageInboxFragmentDirections.ActionInboxToThread action = DirectMessageInboxFragmentDirections
                                        .actionInboxToThread(threadId, threadTitle);
                                controller.navigate(action);
                                controller.removeOnDestinationChangedListener(this);
                            }
                        }
                    });
                    // pop back stack until we reach the inbox page
                    navController.popBackStack(R.id.directMessagesInboxFragment, false);
                } finally {
                    currentNavControllerLiveData.removeObserver(this);
                }
            }
        });
        final int selectedItemId = binding.bottomNavView.getSelectedItemId();
        if (selectedItemId != R.navigation.direct_messages_nav_graph) {
            setBottomNavSelectedTab(R.id.direct_messages_nav_graph);
        }
    }

    private void handleUrl(final String url) {
        if (url == null) return;
        // Log.d(TAG, url);
        final IntentModel intentModel = IntentUtils.parseUrl(url);
        if (intentModel == null) return;
        showView(intentModel);
    }

    private void showView(final IntentModel intentModel) {
        switch (intentModel.getType()) {
            case USERNAME:
                showProfileView(intentModel);
                break;
            case POST:
                showPostView(intentModel);
                break;
            case LOCATION:
                showLocationView(intentModel);
                break;
            case HASHTAG:
                showHashtagView(intentModel);
                break;
            case UNKNOWN:
            default:
                Log.w(TAG, "Unknown model type received!");
        }
    }

    private void showProfileView(@NonNull final IntentModel intentModel) {
        final String username = intentModel.getText();
        // Log.d(TAG, "username: " + username);
        if (currentNavControllerLiveData == null) return;
        final NavController navController = currentNavControllerLiveData.getValue();
        if (navController == null) return;
        final Bundle bundle = new Bundle();
        bundle.putString("username", "@" + username);
        try {
            navController.navigate(R.id.action_global_profileFragment, bundle);
        } catch (Exception e) {
            Log.e(TAG, "showProfileView: ", e);
        }
    }

    private void showPostView(@NonNull final IntentModel intentModel) {
        final String shortCode = intentModel.getText();
        // Log.d(TAG, "shortCode: " + shortCode);
        final AlertDialog alertDialog = new AlertDialog.Builder(this)
                .setCancelable(false)
                .setView(R.layout.dialog_opening_post)
                .create();
        if (graphQLService == null) graphQLService = GraphQLService.getInstance();
        if (mediaService == null) mediaService = MediaService.getInstance(null, null, 0L);
        final ServiceCallback<Media> postCb = new ServiceCallback<Media>() {
            @Override
            public void onSuccess(final Media feedModel){
                if (feedModel != null) {
                    if (currentNavControllerLiveData == null) return;
                    final NavController navController = currentNavControllerLiveData.getValue();
                    if (navController == null) return;
                    final Bundle bundle = new Bundle();
                    bundle.putSerializable(PostViewV2Fragment.ARG_MEDIA, feedModel);
                    try {
                        navController.navigate(R.id.action_global_post_view, bundle);
                    } catch (Exception e) {
                        Log.e(TAG, "showPostView: ", e);
                    }
                }
                Toast.makeText(getApplicationContext(), R.string.post_not_found, Toast.LENGTH_SHORT).show();
                alertDialog.dismiss();
            }

            @Override
            public void onFailure(final Throwable t) {
                alertDialog.dismiss();
            }
        };
        alertDialog.show();
        if (isLoggedIn) mediaService.fetch(TextUtils.shortcodeToId(shortCode), postCb);
        else graphQLService.fetchPost(shortCode, postCb);
    }

    private void showLocationView(@NonNull final IntentModel intentModel) {
        final String locationId = intentModel.getText();
        // Log.d(TAG, "locationId: " + locationId);
        if (currentNavControllerLiveData == null) return;
        final NavController navController = currentNavControllerLiveData.getValue();
        if (navController == null) return;
        final Bundle bundle = new Bundle();
        bundle.putLong("locationId", Long.parseLong(locationId));
        navController.navigate(R.id.action_global_locationFragment, bundle);
    }

    private void showHashtagView(@NonNull final IntentModel intentModel) {
        final String hashtag = intentModel.getText();
        // Log.d(TAG, "hashtag: " + hashtag);
        if (currentNavControllerLiveData == null) return;
        final NavController navController = currentNavControllerLiveData.getValue();
        if (navController == null) return;
        final Bundle bundle = new Bundle();
        bundle.putString("hashtag", hashtag);
        navController.navigate(R.id.action_global_hashTagFragment, bundle);
    }

    private void showActivityView() {
        if (currentNavControllerLiveData == null) return;
        final NavController navController = currentNavControllerLiveData.getValue();
        if (navController == null) return;
        final Bundle bundle = new Bundle();
        bundle.putString("type", "notif");
        navController.navigate(R.id.action_global_notificationsViewerFragment, bundle);
    }

    private void bindActivityCheckerService() {
        bindService(new Intent(this, ActivityCheckerService.class), serviceConnection, Context.BIND_AUTO_CREATE);
        isActivityCheckerServiceBound = true;
    }

    private void unbindActivityCheckerService() {
        if (!isActivityCheckerServiceBound) return;
        unbindService(serviceConnection);
        isActivityCheckerServiceBound = false;
    }

    @NonNull
    public BottomNavigationView getBottomNavView() {
        return binding.bottomNavView;
    }

    public void setCollapsingView(@NonNull final View view) {
        try {
            binding.collapsingToolbarLayout.addView(view, 0);
        } catch (Exception e) {
            Log.e(TAG, "setCollapsingView: ", e);
        }
    }

    public void removeCollapsingView(@NonNull final View view) {
        try {
            binding.collapsingToolbarLayout.removeView(view);
        } catch (Exception e) {
            Log.e(TAG, "removeCollapsingView: ", e);
        }
    }

    public void setToolbar(final Toolbar toolbar) {
        binding.appBarLayout.setVisibility(View.GONE);
        removeScrollingBehaviour();
        setSupportActionBar(toolbar);
        if (currentNavControllerLiveData == null) return;
        setupNavigation(toolbar, currentNavControllerLiveData.getValue());
    }

    public void resetToolbar() {
        binding.appBarLayout.setVisibility(View.VISIBLE);
        setScrollingBehaviour();
        setSupportActionBar(binding.toolbar);
        if (currentNavControllerLiveData == null) return;
        setupNavigation(binding.toolbar, currentNavControllerLiveData.getValue());
    }

    public CollapsingToolbarLayout getCollapsingToolbarView() {
        return binding.collapsingToolbarLayout;
    }

    public AppBarLayout getAppbarLayout() {
        return binding.appBarLayout;
    }

    public void removeLayoutTransition() {
        binding.getRoot().setLayoutTransition(null);
    }

    public void setLayoutTransition() {
        binding.getRoot().setLayoutTransition(new LayoutTransition());
    }

    private void initEmojiCompat() {
        // Use a downloadable font for EmojiCompat
        final FontRequest fontRequest = new FontRequest(
                "com.google.android.gms.fonts",
                "com.google.android.gms",
                "Noto Color Emoji Compat",
                R.array.com_google_android_gms_fonts_certs);
        final EmojiCompat.Config config = new FontRequestEmojiCompatConfig(getApplicationContext(), fontRequest);
        config.setReplaceAll(true)
              // .setUseEmojiAsDefaultStyle(true)
              .registerInitCallback(new EmojiCompat.InitCallback() {
                  @Override
                  public void onInitialized() {
                      Log.i(TAG, "EmojiCompat initialized");
                  }

                  @Override
                  public void onFailed(@Nullable Throwable throwable) {
                      Log.e(TAG, "EmojiCompat initialization failed", throwable);
                  }
              });
        EmojiCompat.init(config);
    }

    public Toolbar getToolbar() {
        return binding.toolbar;
    }

    public View getRootView() {
        return binding.getRoot();
    }

    public List<Tab> getCurrentTabs() {
        return currentTabs;
    }

    private void setNavBarDMUnreadCountBadge(final int unseenCount) {
        final BadgeDrawable badge = binding.bottomNavView.getOrCreateBadge(R.id.direct_messages_nav_graph);
        if (badge == null) return;
        if (unseenCount == 0) {
            badge.setVisible(false);
            badge.clearNumber();
            return;
        }
        if (badge.getVerticalOffset() != 10) {
            badge.setVerticalOffset(10);
        }
        badge.setNumber(unseenCount);
        badge.setVisible(true);
    }

    @NonNull
    public TextInputLayout showSearchView() {
        binding.searchInputLayout.setVisibility(View.VISIBLE);
        return binding.searchInputLayout;
    }

    public void hideSearchView() {
        binding.searchInputLayout.setVisibility(View.GONE);
    }
}