package awais.instagrabber.adapters.viewholder.directmessages;

import android.view.Gravity;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.constraintlayout.widget.ConstraintLayout;

import com.facebook.drawee.generic.GenericDraweeHierarchyBuilder;
import com.facebook.drawee.generic.RoundingParams;
import com.google.common.collect.ImmutableList;

import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectItemsAdapter.DirectItemCallback;
import awais.instagrabber.customviews.DirectItemContextMenu;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmReelShareBinding;
import awais.instagrabber.models.enums.MediaItemType;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemReelShare;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.ResponseBodyUtils;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

public class DirectItemReelShareViewHolder extends DirectItemViewHolder {

    private final LayoutDmReelShareBinding binding;
    private String type;

    public DirectItemReelShareViewHolder(@NonNull final LayoutDmBaseBinding baseBinding,
                                         @NonNull final LayoutDmReelShareBinding binding,
                                         final User currentUser,
                                         final DirectThread thread,
                                         final DirectItemCallback callback) {
        super(baseBinding, currentUser, thread, callback);
        this.binding = binding;
        setItemView(binding.getRoot());
    }

    @Override
    public void bindItem(final DirectItem item, final MessageDirection messageDirection) {
        final DirectItemReelShare reelShare = item.getReelShare();
        type = reelShare.getType();
        if (type == null) return;
        final boolean isSelf = isSelf(item);
        final Media media = reelShare.getMedia();
        if (media == null) return;
        final User user = media.getUser();
        if (user == null) return;
        final boolean expired = media.getMediaType() == null;
        if (expired) {
            binding.preview.setVisibility(View.GONE);
            binding.typeIcon.setVisibility(View.GONE);
            binding.quoteLine.setVisibility(View.GONE);
            binding.reaction.setVisibility(View.GONE);
        } else {
            binding.preview.setVisibility(View.VISIBLE);
            binding.typeIcon.setVisibility(View.VISIBLE);
            binding.quoteLine.setVisibility(View.VISIBLE);
            binding.reaction.setVisibility(View.VISIBLE);
        }
        setGravity(messageDirection, expired);
        if (type.equals("reply")) {
            setReply(messageDirection, reelShare, isSelf);
        }
        if (type.equals("reaction")) {
            setReaction(messageDirection, reelShare, isSelf, expired);
        }
        if (type.equals("mention")) {
            setMention(isSelf);
        }
        if (!expired) {
            setPreview(media);
            itemView.setOnClickListener(v -> openMedia(media));
        }
    }

    private void setGravity(final MessageDirection messageDirection, final boolean expired) {
        final boolean isIncoming = messageDirection == MessageDirection.INCOMING;
        binding.shareInfo.setGravity(isIncoming ? Gravity.START : Gravity.END);
        if (!expired) {
            binding.quoteLine.setVisibility(isIncoming ? View.VISIBLE : View.GONE);
            binding.quoteLineEnd.setVisibility(isIncoming ? View.GONE : View.VISIBLE);
        }
        final ConstraintLayout.LayoutParams layoutParams = (ConstraintLayout.LayoutParams) binding.quoteLine.getLayoutParams();
        layoutParams.horizontalBias = isIncoming ? 0 : 1;
        final ConstraintLayout.LayoutParams messageLayoutParams = (ConstraintLayout.LayoutParams) binding.message.getLayoutParams();
        messageLayoutParams.startToStart = isIncoming ? ConstraintLayout.LayoutParams.PARENT_ID : ConstraintLayout.LayoutParams.UNSET;
        messageLayoutParams.endToEnd = isIncoming ? ConstraintLayout.LayoutParams.UNSET : ConstraintLayout.LayoutParams.PARENT_ID;
        messageLayoutParams.setMarginStart(isIncoming ? messageInfoPaddingSmall : 0);
        messageLayoutParams.setMarginEnd(isIncoming ? 0 : messageInfoPaddingSmall);
        final ConstraintLayout.LayoutParams reactionLayoutParams = (ConstraintLayout.LayoutParams) binding.reaction.getLayoutParams();
        final int previewId = binding.preview.getId();
        if (isIncoming) {
            reactionLayoutParams.startToEnd = previewId;
            reactionLayoutParams.endToEnd = previewId;
            reactionLayoutParams.startToStart = ConstraintLayout.LayoutParams.UNSET;
            reactionLayoutParams.endToStart = ConstraintLayout.LayoutParams.UNSET;
        } else {
            reactionLayoutParams.startToStart = previewId;
            reactionLayoutParams.endToStart = previewId;
            reactionLayoutParams.startToEnd = ConstraintLayout.LayoutParams.UNSET;
            reactionLayoutParams.endToEnd = ConstraintLayout.LayoutParams.UNSET;
        }
    }

    private void setReply(final MessageDirection messageDirection,
                          final DirectItemReelShare reelShare,
                          final boolean isSelf) {
        final int info = isSelf ? R.string.replied_story_outgoing : R.string.replied_story_incoming;
        binding.shareInfo.setText(info);
        binding.reaction.setVisibility(View.GONE);
        final String text = reelShare.getText();
        if (TextUtils.isEmpty(text)) {
            binding.message.setVisibility(View.GONE);
            return;
        }
        setMessage(messageDirection, text);
    }

    private void setReaction(final MessageDirection messageDirection,
                             final DirectItemReelShare reelShare,
                             final boolean isSelf,
                             final boolean expired) {
        final int info = isSelf ? R.string.reacted_story_outgoing : R.string.reacted_story_incoming;
        binding.shareInfo.setText(info);
        binding.message.setVisibility(View.GONE);
        final String text = reelShare.getText();
        if (TextUtils.isEmpty(text)) {
            binding.reaction.setVisibility(View.GONE);
            return;
        }
        if (expired) {
            setMessage(messageDirection, text);
            return;
        }
        binding.reaction.setVisibility(View.VISIBLE);
        binding.reaction.setText(text);
    }

    private void setMention(final boolean isSelf) {
        final int info = isSelf ? R.string.mentioned_story_outgoing : R.string.mentioned_story_incoming;
        binding.shareInfo.setText(info);
        binding.message.setVisibility(View.GONE);
        binding.reaction.setVisibility(View.GONE);
    }

    private void setMessage(final MessageDirection messageDirection, final String text) {
        binding.message.setVisibility(View.VISIBLE);
        binding.message.setBackgroundResource(messageDirection == MessageDirection.INCOMING
                                              ? R.drawable.bg_speech_bubble_incoming
                                              : R.drawable.bg_speech_bubble_outgoing);
        binding.message.setText(text);
    }

    private void setPreview(final Media media) {
        final MediaItemType mediaType = media.getMediaType();
        if (mediaType == null) return;
        binding.typeIcon.setVisibility(mediaType == MediaItemType.MEDIA_TYPE_VIDEO || mediaType == MediaItemType.MEDIA_TYPE_SLIDER
                                       ? View.VISIBLE : View.GONE);
        final RoundingParams roundingParams = RoundingParams.fromCornersRadii(dmRadiusSmall, dmRadiusSmall, dmRadiusSmall, dmRadiusSmall);
        binding.preview.setHierarchy(new GenericDraweeHierarchyBuilder(itemView.getResources())
                                             .setRoundingParams(roundingParams)
                                             .build());
        final String thumbUrl = ResponseBodyUtils.getThumbUrl(media);
        binding.preview.setImageURI(thumbUrl);
    }

    @Override
    protected boolean canForward() {
        return false;
    }

    @Override
    protected List<DirectItemContextMenu.MenuItem> getLongClickOptions() {
        final ImmutableList.Builder<DirectItemContextMenu.MenuItem> builder = ImmutableList.builder();
        if (type != null && type.equals("reply")) {
            builder.add(new DirectItemContextMenu.MenuItem(R.id.copy, R.string.copy_reply, item -> {
                final DirectItemReelShare reelShare = item.getReelShare();
                if (reelShare == null) return null;
                final String text = reelShare.getText();
                if (TextUtils.isEmpty(text)) return null;
                Utils.copyText(itemView.getContext(), text);
                return null;
            }));
        }
        return builder.build();
    }
}
