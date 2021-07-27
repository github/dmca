package awais.instagrabber.adapters.viewholder.feed;

import android.text.method.LinkMovementMethod;
import android.transition.TransitionManager;
import android.view.View;
import android.widget.RelativeLayout;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.adapters.FeedAdapterV2;
import awais.instagrabber.databinding.ItemFeedBottomBinding;
import awais.instagrabber.databinding.ItemFeedTopBinding;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Caption;
import awais.instagrabber.repositories.responses.Location;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.TextUtils;

import static android.text.TextUtils.TruncateAt.END;

public abstract class FeedItemViewHolder extends RecyclerView.ViewHolder {
    public static final int MAX_LINES_COLLAPSED = 5;
    private final ItemFeedTopBinding topBinding;
    private final ItemFeedBottomBinding bottomBinding;
    private final FeedAdapterV2.FeedItemCallback feedItemCallback;

    public FeedItemViewHolder(@NonNull final View root,
                              final ItemFeedTopBinding topBinding,
                              final ItemFeedBottomBinding bottomBinding,
                              final FeedAdapterV2.FeedItemCallback feedItemCallback) {
        super(root);
        this.topBinding = topBinding;
        this.bottomBinding = bottomBinding;
        topBinding.title.setMovementMethod(new LinkMovementMethod());
        this.feedItemCallback = feedItemCallback;
    }

    public void bind(final Media media) {
        if (media == null) {
            return;
        }
        setupProfilePic(media);
        setupLocation(media);
        bottomBinding.tvPostDate.setText(media.getDate());
        setupComments(media);
        setupCaption(media);
        if (media.getMediaType() != MediaItemType.MEDIA_TYPE_SLIDER) {
            bottomBinding.btnDownload.setOnClickListener(v -> feedItemCallback.onDownloadClick(media, -1));
        }
        bindItem(media);
    }

    private void setupComments(@NonNull final Media feedModel) {
        final long commentsCount = feedModel.getCommentCount();
        bottomBinding.commentsCount.setText(String.valueOf(commentsCount));
        bottomBinding.btnComments.setOnClickListener(v -> feedItemCallback.onCommentsClick(feedModel));
    }

    private void setupProfilePic(@NonNull final Media media) {
        final User user = media.getUser();
        if (user == null) {
            topBinding.ivProfilePic.setVisibility(View.GONE);
            topBinding.title.setVisibility(View.GONE);
            return;
        }
        topBinding.ivProfilePic.setOnClickListener(v -> feedItemCallback.onProfilePicClick(media, topBinding.ivProfilePic));
        topBinding.ivProfilePic.setImageURI(user.getProfilePicUrl());
        setupTitle(media);
    }

    private void setupTitle(@NonNull final Media media) {
        // final int titleLen = profileModel.getUsername().length() + 1;
        // final SpannableString spannableString = new SpannableString();
        // spannableString.setSpan(new CommentMentionClickSpan(), 0, titleLen, 0);
        final User user = media.getUser();
        if (user == null) return;
        final String title = "@" + user.getUsername();
        topBinding.title.setText(title);
        topBinding.title.setOnClickListener(v -> feedItemCallback.onNameClick(media, topBinding.ivProfilePic));
    }

    private void setupCaption(final Media media) {
        bottomBinding.viewerCaption.clearOnMentionClickListeners();
        bottomBinding.viewerCaption.clearOnHashtagClickListeners();
        bottomBinding.viewerCaption.clearOnURLClickListeners();
        bottomBinding.viewerCaption.clearOnEmailClickListeners();
        final Caption caption = media.getCaption();
        if (caption == null) {
            bottomBinding.viewerCaption.setVisibility(View.GONE);
            return;
        }
        final CharSequence postCaption = caption.getText();
        final boolean captionEmpty = TextUtils.isEmpty(postCaption);
        bottomBinding.viewerCaption.setVisibility(captionEmpty ? View.GONE : View.VISIBLE);
        if (captionEmpty) return;
        bottomBinding.viewerCaption.setText(postCaption);
        bottomBinding.viewerCaption.setMaxLines(MAX_LINES_COLLAPSED);
        bottomBinding.viewerCaption.setEllipsize(END);
        bottomBinding.viewerCaption.setOnClickListener(v -> bottomBinding.getRoot().post(() -> {
            TransitionManager.beginDelayedTransition(bottomBinding.getRoot());
            if (bottomBinding.viewerCaption.getMaxLines() == MAX_LINES_COLLAPSED) {
                bottomBinding.viewerCaption.setMaxLines(Integer.MAX_VALUE);
                bottomBinding.viewerCaption.setEllipsize(null);
                return;
            }
            bottomBinding.viewerCaption.setMaxLines(MAX_LINES_COLLAPSED);
            bottomBinding.viewerCaption.setEllipsize(END);
        }));
        bottomBinding.viewerCaption.addOnMentionClickListener(autoLinkItem -> feedItemCallback.onMentionClick(autoLinkItem.getOriginalText()));
        bottomBinding.viewerCaption.addOnHashtagListener(autoLinkItem -> feedItemCallback.onHashtagClick(autoLinkItem.getOriginalText()));
        bottomBinding.viewerCaption.addOnEmailClickListener(autoLinkItem -> feedItemCallback.onEmailClick(autoLinkItem.getOriginalText()));
        bottomBinding.viewerCaption.addOnURLClickListener(autoLinkItem -> feedItemCallback.onURLClick(autoLinkItem.getOriginalText()));
    }

    private void setupLocation(@NonNull final Media media) {
        final Location location = media.getLocation();
        if (location == null) {
            topBinding.location.setVisibility(View.GONE);
            topBinding.title.setLayoutParams(new RelativeLayout.LayoutParams(
                    RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT
            ));
        } else {
            final String locationName = location.getName();
            if (TextUtils.isEmpty(locationName)) {
                topBinding.location.setVisibility(View.GONE);
                topBinding.title.setLayoutParams(new RelativeLayout.LayoutParams(
                        RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.MATCH_PARENT
                ));
            } else {
                topBinding.location.setVisibility(View.VISIBLE);
                topBinding.location.setText(locationName);
                topBinding.title.setLayoutParams(new RelativeLayout.LayoutParams(
                        RelativeLayout.LayoutParams.MATCH_PARENT, RelativeLayout.LayoutParams.WRAP_CONTENT
                ));
                topBinding.location.setOnClickListener(v -> feedItemCallback.onLocationClick(media));
            }
        }
    }

    public abstract void bindItem(final Media media);
}