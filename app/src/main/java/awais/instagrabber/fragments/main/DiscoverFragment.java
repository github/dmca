package awais.instagrabber.fragments.main;

import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.coordinatorlayout.widget.CoordinatorLayout;
import androidx.fragment.app.Fragment;
import androidx.lifecycle.ViewModelProvider;
import androidx.navigation.NavController;
import androidx.navigation.fragment.FragmentNavigator;
import androidx.navigation.fragment.NavHostFragment;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import java.util.Collections;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.activities.MainActivity;
import awais.instagrabber.adapters.DiscoverTopicsAdapter;
import awais.instagrabber.customviews.helpers.GridSpacingItemDecoration;
import awais.instagrabber.databinding.FragmentDiscoverBinding;
import awais.instagrabber.fragments.PostViewV2Fragment;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.discover.TopicCluster;
import awais.instagrabber.repositories.responses.discover.TopicalExploreFeedResponse;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.viewmodels.TopicClusterViewModel;
import awais.instagrabber.webservices.DiscoverService;
import awais.instagrabber.webservices.MediaService;
import awais.instagrabber.webservices.ServiceCallback;

public class DiscoverFragment extends Fragment implements SwipeRefreshLayout.OnRefreshListener {
    private static final String TAG = "DiscoverFragment";

    private MainActivity fragmentActivity;
    private CoordinatorLayout root;
    private FragmentDiscoverBinding binding;
    private TopicClusterViewModel topicClusterViewModel;
    private boolean shouldRefresh = true;
    private DiscoverService discoverService;
    private MediaService mediaService;

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        fragmentActivity = (MainActivity) requireActivity();
        discoverService = DiscoverService.getInstance();
        mediaService = MediaService.getInstance(null, null, 0);
    }

    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater,
                             final ViewGroup container,
                             final Bundle savedInstanceState) {
        if (root != null) {
            shouldRefresh = false;
            return root;
        }
        binding = FragmentDiscoverBinding.inflate(inflater, container, false);
        root = binding.getRoot();
        return root;
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        if (!shouldRefresh) return;
        binding.swipeRefreshLayout.setOnRefreshListener(this);
        init();
        shouldRefresh = false;
    }

    private void init() {
        setupTopics();
        fetchTopics();
    }

    @Override
    public void onRefresh() {
        fetchTopics();
    }

    public void setupTopics() {
        topicClusterViewModel = new ViewModelProvider(fragmentActivity).get(TopicClusterViewModel.class);
        binding.topicsRecyclerView.addItemDecoration(new GridSpacingItemDecoration(Utils.convertDpToPx(2)));
        final DiscoverTopicsAdapter.OnTopicClickListener otcl = new DiscoverTopicsAdapter.OnTopicClickListener() {
            public void onTopicClick(final TopicCluster topicCluster, final View cover, final int titleColor, final int backgroundColor) {
                final FragmentNavigator.Extras.Builder builder = new FragmentNavigator.Extras.Builder()
                        .addSharedElement(cover, "cover-" + topicCluster.getId());
                final DiscoverFragmentDirections.ActionDiscoverFragmentToTopicPostsFragment action = DiscoverFragmentDirections
                        .actionDiscoverFragmentToTopicPostsFragment(topicCluster, titleColor, backgroundColor);
                NavHostFragment.findNavController(DiscoverFragment.this).navigate(action, builder.build());
            }

            public void onTopicLongClick(final Media coverMedia) {
                final AlertDialog alertDialog = new AlertDialog.Builder(requireContext())
                        .setCancelable(false)
                        .setView(R.layout.dialog_opening_post)
                        .create();
                alertDialog.show();
                mediaService.fetch(Long.valueOf(coverMedia.getPk()), new ServiceCallback<Media>() {
                    @Override
                    public void onSuccess(final Media feedModel) {
                        final NavController navController = NavHostFragment.findNavController(DiscoverFragment.this);
                        final Bundle bundle = new Bundle();
                        bundle.putSerializable(PostViewV2Fragment.ARG_MEDIA, feedModel);
                        try {
                            navController.navigate(R.id.action_global_post_view, bundle);
                            alertDialog.dismiss();
                        } catch (Exception e) {
                            Log.e(TAG, "onSuccess: ", e);
                        }
                    }

                    @Override
                    public void onFailure(final Throwable t) {
                        alertDialog.dismiss();
                        try {
                            Toast.makeText(requireContext(), R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                        }
                        catch (Throwable e) {}
                    }
                });
            }
        };
        final DiscoverTopicsAdapter adapter = new DiscoverTopicsAdapter(otcl);
        binding.topicsRecyclerView.setAdapter(adapter);
        topicClusterViewModel.getList().observe(getViewLifecycleOwner(), adapter::submitList);
    }

    private void fetchTopics() {
        binding.swipeRefreshLayout.setRefreshing(true);
        discoverService.topicalExplore(new DiscoverService.TopicalExploreRequest(), new ServiceCallback<TopicalExploreFeedResponse>() {
            @Override
            public void onSuccess(final TopicalExploreFeedResponse result) {
                if (result == null) return;
                final List<TopicCluster> clusters = result.getClusters();
                binding.swipeRefreshLayout.setRefreshing(false);
                if (clusters.size() == 1 && result.getItems().size() > 0) {
                    final TopicCluster cluster = clusters.get(0);
                    if (cluster.getCoverMedia() == null)
                        cluster.setCoverMedia(result.getItems().get(0).getMedia());
                    topicClusterViewModel.getList().postValue(Collections.singletonList(cluster));
                    return;
                }
                if (clusters.size() > 1 || result.getItems().size() == 0) {
                    topicClusterViewModel.getList().postValue(clusters);
                }
            }

            @Override
            public void onFailure(final Throwable t) {
                Log.e(TAG, "onFailure", t);
                binding.swipeRefreshLayout.setRefreshing(false);
            }
        });
    }
}
