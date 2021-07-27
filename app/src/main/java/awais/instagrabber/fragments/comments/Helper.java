package awais.instagrabber.fragments.comments;

import android.content.Context;
import android.os.Bundle;
import android.text.Editable;
import android.util.Log;
import android.view.View;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
import androidx.lifecycle.LifecycleOwner;
import androidx.lifecycle.LiveData;
import androidx.lifecycle.Observer;
import androidx.navigation.NavController;
import androidx.navigation.NavDirections;
import androidx.recyclerview.widget.DividerItemDecoration;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.dialog.MaterialAlertDialogBuilder;
import com.google.android.material.internal.CheckableImageButton;
import com.google.android.material.textfield.TextInputEditText;
import com.google.android.material.textfield.TextInputLayout;

import java.util.function.BiFunction;
import java.util.function.Function;

import awais.instagrabber.R;
import awais.instagrabber.adapters.CommentsAdapter.CommentCallback;
import awais.instagrabber.customviews.helpers.TextWatcherAdapter;
import awais.instagrabber.models.Comment;
import awais.instagrabber.models.Resource;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;
import awais.instagrabber.viewmodels.CommentsViewerViewModel;
import awais.instagrabber.webservices.ServiceCallback;

public final class Helper {
    private static final String TAG = Helper.class.getSimpleName();

    public static void setupList(@NonNull final Context context,
                                 @NonNull final RecyclerView list,
                                 @NonNull final RecyclerView.LayoutManager layoutManager,
                                 @NonNull final RecyclerView.OnScrollListener lazyLoader) {
        list.setLayoutManager(layoutManager);
        final DividerItemDecoration itemDecoration = new DividerItemDecoration(context, LinearLayoutManager.VERTICAL);
        itemDecoration.setDrawable(ContextCompat.getDrawable(context, R.drawable.pref_list_divider_material));
        list.addItemDecoration(itemDecoration);
        list.addOnScrollListener(lazyLoader);
    }

    @NonNull
    public static CommentCallback getCommentCallback(@NonNull final Context context,
                                                     final LifecycleOwner lifecycleOwner,
                                                     final NavController navController,
                                                     @NonNull final CommentsViewerViewModel viewModel,
                                                     final BiFunction<Comment, Boolean, Void> onRepliesClick) {
        return new CommentCallback() {
            @Override
            public void onClick(final Comment comment) {
                // onCommentClick(comment);
                if (onRepliesClick == null) return;
                onRepliesClick.apply(comment, false);
            }

            @Override
            public void onHashtagClick(final String hashtag) {
                try {
                    if (navController == null) return;
                    final NavDirections action = CommentsViewerFragmentDirections.actionGlobalHashTagFragment(hashtag);
                    navController.navigate(action);
                } catch (Exception e) {
                    Log.e(TAG, "onHashtagClick: ", e);
                }
            }

            @Override
            public void onMentionClick(final String mention) {
                openProfile(navController, mention);
            }

            @Override
            public void onURLClick(final String url) {
                Utils.openURL(context, url);
            }

            @Override
            public void onEmailClick(final String emailAddress) {
                Utils.openEmailAddress(context, emailAddress);
            }

            @Override
            public void onLikeClick(final Comment comment, final boolean liked, final boolean isReply) {
                if (comment == null) return;
                final LiveData<Resource<Object>> resourceLiveData = viewModel.likeComment(comment, liked, isReply);
                resourceLiveData.observe(lifecycleOwner, new Observer<Resource<Object>>() {
                    @Override
                    public void onChanged(final Resource<Object> objectResource) {
                        if (objectResource == null) return;
                        switch (objectResource.status) {
                            case SUCCESS:
                                resourceLiveData.removeObserver(this);
                                break;
                            case LOADING:
                                break;
                            case ERROR:
                                if (objectResource.message != null) {
                                    Toast.makeText(context, objectResource.message, Toast.LENGTH_LONG).show();
                                }
                                resourceLiveData.removeObserver(this);
                        }
                    }
                });
            }

            @Override
            public void onRepliesClick(final Comment comment) {
                // viewModel.showReplies(comment);
                if (onRepliesClick == null) return;
                onRepliesClick.apply(comment, true);
            }

            @Override
            public void onViewLikes(final Comment comment) {
                if (navController == null) return;
                try {
                    final Bundle bundle = new Bundle();
                    bundle.putString("postId", comment.getId());
                    bundle.putBoolean("isComment", true);
                    navController.navigate(R.id.action_global_likesViewerFragment, bundle);
                } catch (Exception e) {
                    Log.e(TAG, "onViewLikes: ", e);
                }
            }

            @Override
            public void onTranslate(final Comment comment) {
                if (comment == null) return;
                viewModel.translate(comment, new ServiceCallback<String>() {
                    @Override
                    public void onSuccess(final String result) {
                        if (TextUtils.isEmpty(result)) {
                            Toast.makeText(context, R.string.downloader_unknown_error, Toast.LENGTH_SHORT).show();
                            return;
                        }
                        String username = "";
                        if (comment.getUser() != null) {
                            username = comment.getUser().getUsername();
                        }
                        new MaterialAlertDialogBuilder(context)
                                .setTitle(username)
                                .setMessage(result)
                                .setPositiveButton(R.string.ok, null)
                                .show();
                    }

                    @Override
                    public void onFailure(final Throwable t) {
                        Log.e(TAG, "Error translating comment", t);
                        Toast.makeText(context, t.getMessage(), Toast.LENGTH_SHORT).show();
                    }
                });
            }

            @Override
            public void onDelete(final Comment comment, final boolean isReply) {
                if (comment == null) return;
                final LiveData<Resource<Object>> resourceLiveData = viewModel.deleteComment(comment, isReply);
                resourceLiveData.observe(lifecycleOwner, new Observer<Resource<Object>>() {
                    @Override
                    public void onChanged(final Resource<Object> objectResource) {
                        if (objectResource == null) return;
                        switch (objectResource.status) {
                            case SUCCESS:
                                resourceLiveData.removeObserver(this);
                                break;
                            case ERROR:
                                if (objectResource.message != null) {
                                    Toast.makeText(context, objectResource.message, Toast.LENGTH_LONG).show();
                                }
                                resourceLiveData.removeObserver(this);
                                break;
                            case LOADING:
                                break;
                        }
                    }
                });
            }
        };
    }

    private static void openProfile(final NavController navController,
                                    @NonNull final String username) {
        if (navController == null) return;
        try {
            final NavDirections action = CommentsViewerFragmentDirections.actionGlobalProfileFragment(username);
            navController.navigate(action);
        } catch (Exception e) {
            Log.e(TAG, "openProfile: ", e);
        }
    }

    public static void setupCommentInput(@NonNull final TextInputLayout commentField,
                                         @NonNull final TextInputEditText commentText,
                                         final boolean isReplyFragment,
                                         @NonNull final Function<String, Void> commentFunction) {
        // commentField.setStartIconVisible(false);
        commentField.setVisibility(View.VISIBLE);
        commentField.setEndIconVisible(false);
        if (isReplyFragment) {
            commentField.setHint(R.string.reply_hint);
        }
        commentText.addTextChangedListener(new TextWatcherAdapter() {
            @Override
            public void onTextChanged(final CharSequence s, final int start, final int before, final int count) {
                final boolean isEmpty = TextUtils.isEmpty(s);
                commentField.setStartIconVisible(!isEmpty);
                commentField.setEndIconVisible(!isEmpty);
                commentField.setCounterEnabled(s != null && s.length() > 2000); // show the counter when user approaches the limit
            }
        });
        // commentField.setStartIconOnClickListener(v -> {
        //     // commentsAdapter.clearSelection();
        //     commentText.setText("");
        // });
        commentField.setEndIconOnClickListener(v -> {
            final Editable text = commentText.getText();
            if (TextUtils.isEmpty(text)) return;
            commentFunction.apply(text.toString().trim());
        });
    }

    public static void handleCommentResource(@NonNull final Context context,
                                             @NonNull final Resource.Status status,
                                             final String message,
                                             @NonNull final LiveData<Resource<Object>> resourceLiveData,
                                             @NonNull final Observer<Resource<Object>> observer,
                                             @NonNull final TextInputLayout commentField,
                                             @NonNull final TextInputEditText commentText,
                                             @NonNull final RecyclerView comments) {
        CheckableImageButton endIcon = null;
        try {
            endIcon = (CheckableImageButton) commentField.findViewById(com.google.android.material.R.id.text_input_end_icon);
        } catch (Exception e) {
            Log.e(TAG, "setupObservers: ", e);
        }
        CheckableImageButton startIcon = null;
        try {
            startIcon = (CheckableImageButton) commentField.findViewById(com.google.android.material.R.id.text_input_start_icon);
        } catch (Exception e) {
            Log.e(TAG, "setupObservers: ", e);
        }
        switch (status) {
            case SUCCESS:
                resourceLiveData.removeObserver(observer);
                comments.postDelayed(() -> comments.scrollToPosition(0), 500);
                if (startIcon != null) {
                    startIcon.setEnabled(true);
                }
                if (endIcon != null) {
                    endIcon.setEnabled(true);
                }
                commentText.setText("");
                break;
            case LOADING:
                commentText.setEnabled(false);
                if (startIcon != null) {
                    startIcon.setEnabled(false);
                }
                if (endIcon != null) {
                    endIcon.setEnabled(false);
                }
                break;
            case ERROR:
                if (message != null && context != null) {
                    Toast.makeText(context, message, Toast.LENGTH_LONG).show();
                }
                if (startIcon != null) {
                    startIcon.setEnabled(true);
                }
                if (endIcon != null) {
                    endIcon.setEnabled(true);
                }
                resourceLiveData.removeObserver(observer);
        }
    }
}
