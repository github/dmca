package awais.instagrabber.adapters;

import android.text.format.DateFormat;
import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.IdRes;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.AdapterListUpdateCallback;
import androidx.recyclerview.widget.AsyncDifferConfig;
import androidx.recyclerview.widget.AsyncListDiffer;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.Objects;
import java.util.function.Function;

import awais.instagrabber.adapters.viewholder.directmessages.DirectItemActionLogViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemAnimatedMediaViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemDefaultViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemLikeViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemLinkViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemMediaShareViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemMediaViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemPlaceholderViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemProfileViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemRavenMediaViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemReelShareViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemStoryShareViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemTextViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemVideoCallEventViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemVoiceMediaViewHolder;
import awais.instagrabber.adapters.viewholder.directmessages.DirectItemXmaViewHolder;
import awais.instagrabber.customviews.emoji.Emoji;
import awais.instagrabber.databinding.LayoutDmActionLogBinding;
import awais.instagrabber.databinding.LayoutDmAnimatedMediaBinding;
import awais.instagrabber.databinding.LayoutDmBaseBinding;
import awais.instagrabber.databinding.LayoutDmHeaderBinding;
import awais.instagrabber.databinding.LayoutDmLikeBinding;
import awais.instagrabber.databinding.LayoutDmLinkBinding;
import awais.instagrabber.databinding.LayoutDmMediaBinding;
import awais.instagrabber.databinding.LayoutDmMediaShareBinding;
import awais.instagrabber.databinding.LayoutDmProfileBinding;
import awais.instagrabber.databinding.LayoutDmRavenMediaBinding;
import awais.instagrabber.databinding.LayoutDmReelShareBinding;
import awais.instagrabber.databinding.LayoutDmStoryShareBinding;
import awais.instagrabber.databinding.LayoutDmTextBinding;
import awais.instagrabber.databinding.LayoutDmVoiceMediaBinding;
import awais.instagrabber.models.enums.DirectItemType;
import awais.instagrabber.repositories.responses.Media;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItem;
import awais.instagrabber.repositories.responses.directmessages.DirectItemStoryShare;
import awais.instagrabber.repositories.responses.directmessages.DirectThread;
import awais.instagrabber.utils.DateUtils;

public final class DirectItemsAdapter extends RecyclerView.Adapter<RecyclerView.ViewHolder> {
    private static final String TAG = DirectItemsAdapter.class.getSimpleName();

    private List<DirectItem> items;
    private DirectThread thread;
    private DirectItemViewHolder selectedViewHolder;

    private final User currentUser;
    private final DirectItemCallback callback;
    private final AsyncListDiffer<DirectItemOrHeader> differ;
    private final DirectItemInternalLongClickListener longClickListener;

    private static final DiffUtil.ItemCallback<DirectItemOrHeader> diffCallback = new DiffUtil.ItemCallback<DirectItemOrHeader>() {
        @Override
        public boolean areItemsTheSame(@NonNull final DirectItemOrHeader oldItem, @NonNull final DirectItemOrHeader newItem) {
            final boolean bothHeaders = oldItem.isHeader() && newItem.isHeader();
            final boolean bothItems = !oldItem.isHeader() && !newItem.isHeader();
            boolean areSameType = bothHeaders || bothItems;
            if (!areSameType) return false;
            if (bothHeaders) {
                return oldItem.date.equals(newItem.date);
            }
            if (oldItem.item != null && newItem.item != null) {
                String oldClientContext = oldItem.item.getClientContext();
                if (oldClientContext == null) {
                    oldClientContext = oldItem.item.getItemId();
                }
                String newClientContext = newItem.item.getClientContext();
                if (newClientContext == null) {
                    newClientContext = newItem.item.getItemId();
                }
                return oldClientContext.equals(newClientContext);
            }
            return false;
        }

        @Override
        public boolean areContentsTheSame(@NonNull final DirectItemOrHeader oldItem, @NonNull final DirectItemOrHeader newItem) {
            final boolean bothHeaders = oldItem.isHeader() && newItem.isHeader();
            final boolean bothItems = !oldItem.isHeader() && !newItem.isHeader();
            boolean areSameType = bothHeaders || bothItems;
            if (!areSameType) return false;
            if (bothHeaders) {
                return oldItem.date.equals(newItem.date);
            }
            final boolean timestampEqual = oldItem.item.getTimestamp() == newItem.item.getTimestamp();
            final boolean bothPending = oldItem.item.isPending() == newItem.item.isPending();
            final boolean reactionSame = Objects.equals(oldItem.item.getReactions(), newItem.item.getReactions());
            return timestampEqual && bothPending && reactionSame;
        }
    };

    public DirectItemsAdapter(@NonNull final User currentUser,
                              @NonNull final DirectThread thread,
                              @NonNull final DirectItemCallback callback,
                              @NonNull final DirectItemLongClickListener itemLongClickListener) {
        this.currentUser = currentUser;
        this.thread = thread;
        this.callback = callback;
        differ = new AsyncListDiffer<>(new AdapterListUpdateCallback(this),
                                       new AsyncDifferConfig.Builder<>(diffCallback).build());
        longClickListener = (position, viewHolder) -> {
            if (selectedViewHolder != null) {
                selectedViewHolder.setSelected(false);
            }
            selectedViewHolder = viewHolder;
            viewHolder.setSelected(true);
            itemLongClickListener.onLongClick(position);
        };
    }

    @NonNull
    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int type) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        if (type == -1) {
            // header
            return new HeaderViewHolder(LayoutDmHeaderBinding.inflate(layoutInflater, parent, false));
        }
        final LayoutDmBaseBinding baseBinding = LayoutDmBaseBinding.inflate(layoutInflater, parent, false);
        final DirectItemType directItemType = DirectItemType.valueOf(type);
        final DirectItemViewHolder itemViewHolder = getItemViewHolder(layoutInflater, baseBinding, directItemType);
        itemViewHolder.setLongClickListener(longClickListener);
        return itemViewHolder;
    }

    @NonNull
    private DirectItemViewHolder getItemViewHolder(final LayoutInflater layoutInflater,
                                                   final LayoutDmBaseBinding baseBinding,
                                                   @NonNull final DirectItemType directItemType) {
        switch (directItemType) {
            case TEXT: {
                final LayoutDmTextBinding binding = LayoutDmTextBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemTextViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case LIKE: {
                final LayoutDmLikeBinding binding = LayoutDmLikeBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemLikeViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case LINK: {
                final LayoutDmLinkBinding binding = LayoutDmLinkBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemLinkViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case ACTION_LOG: {
                final LayoutDmActionLogBinding binding = LayoutDmActionLogBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemActionLogViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case VIDEO_CALL_EVENT: {
                final LayoutDmActionLogBinding binding = LayoutDmActionLogBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemVideoCallEventViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case PLACEHOLDER: {
                final LayoutDmTextBinding binding = LayoutDmTextBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemPlaceholderViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case ANIMATED_MEDIA: {
                final LayoutDmAnimatedMediaBinding binding = LayoutDmAnimatedMediaBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemAnimatedMediaViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case VOICE_MEDIA: {
                final LayoutDmVoiceMediaBinding binding = LayoutDmVoiceMediaBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemVoiceMediaViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case LOCATION:
            case PROFILE: {
                final LayoutDmProfileBinding binding = LayoutDmProfileBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemProfileViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case MEDIA: {
                final LayoutDmMediaBinding binding = LayoutDmMediaBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemMediaViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case CLIP:
            case FELIX_SHARE:
            case MEDIA_SHARE: {
                final LayoutDmMediaShareBinding binding = LayoutDmMediaShareBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemMediaShareViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case STORY_SHARE: {
                final LayoutDmStoryShareBinding binding = LayoutDmStoryShareBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemStoryShareViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case REEL_SHARE: {
                final LayoutDmReelShareBinding binding = LayoutDmReelShareBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemReelShareViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case RAVEN_MEDIA: {
                final LayoutDmRavenMediaBinding binding = LayoutDmRavenMediaBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemRavenMediaViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case XMA: {
                final LayoutDmAnimatedMediaBinding binding = LayoutDmAnimatedMediaBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemXmaViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
            case UNKNOWN:
            default: {
                final LayoutDmTextBinding binding = LayoutDmTextBinding.inflate(layoutInflater, baseBinding.message, false);
                return new DirectItemDefaultViewHolder(baseBinding, binding, currentUser, thread, callback);
            }
        }
    }

    @Override
    public void onBindViewHolder(@NonNull final RecyclerView.ViewHolder holder, final int position) {
        final DirectItemOrHeader itemOrHeader = getItem(position);
        if (itemOrHeader.isHeader()) {
            ((HeaderViewHolder) holder).bind(itemOrHeader.date);
            return;
        }
        if (thread == null) return;
        ((DirectItemViewHolder) holder).bind(position, itemOrHeader.item);
    }

    protected DirectItemOrHeader getItem(int position) {
        return differ.getCurrentList().get(position);
    }

    @Override
    public int getItemCount() {
        return differ.getCurrentList().size();
    }

    @Override
    public int getItemViewType(final int position) {
        final DirectItemOrHeader itemOrHeader = getItem(position);
        if (itemOrHeader.isHeader()) {
            return -1;
        }
        final DirectItemType itemType = itemOrHeader.item.getItemType();
        if (itemType == null) {
            return 0;
        }
        return itemType.getId();
    }

    @Override
    public long getItemId(final int position) {
        final DirectItemOrHeader itemOrHeader = getItem(position);
        if (itemOrHeader.isHeader()) {
            return itemOrHeader.date.hashCode();
        }
        if (itemOrHeader.item.getClientContext() == null) {
            return itemOrHeader.item.getItemId().hashCode();
        }
        return itemOrHeader.item.getClientContext().hashCode();
    }

    public void setThread(final DirectThread thread) {
        if (thread == null) return;
        this.thread = thread;
        // notifyDataSetChanged();
    }

    public void submitList(@Nullable final List<DirectItem> list) {
        if (list == null) {
            differ.submitList(null);
            return;
        }
        differ.submitList(sectionAndSort(list));
        this.items = list;
    }

    public void submitList(@Nullable final List<DirectItem> list, @Nullable final Runnable commitCallback) {
        if (list == null) {
            differ.submitList(null, commitCallback);
            return;
        }
        differ.submitList(sectionAndSort(list), commitCallback);
        this.items = list;
    }

    private List<DirectItemOrHeader> sectionAndSort(final List<DirectItem> list) {
        final List<DirectItemOrHeader> itemOrHeaders = new ArrayList<>();
        Date prevSectionDate = null;
        for (int i = 0; i < list.size(); i++) {
            final DirectItem item = list.get(i);
            if (item == null) continue;
            final DirectItemOrHeader prev = itemOrHeaders.isEmpty() ? null : itemOrHeaders.get(itemOrHeaders.size() - 1);
            if (prev != null && prev.item != null && DateUtils.isSameDay(prev.item.getDate(), item.getDate())) {
                // just add item
                final DirectItemOrHeader itemOrHeader = new DirectItemOrHeader();
                itemOrHeader.item = item;
                itemOrHeaders.add(itemOrHeader);
                if (i == list.size() - 1) {
                    // add header
                    final DirectItemOrHeader itemOrHeader2 = new DirectItemOrHeader();
                    itemOrHeader2.date = prevSectionDate;
                    itemOrHeaders.add(itemOrHeader2);
                }
                continue;
            }
            if (prevSectionDate != null) {
                // add header
                final DirectItemOrHeader itemOrHeader = new DirectItemOrHeader();
                itemOrHeader.date = prevSectionDate;
                itemOrHeaders.add(itemOrHeader);
            }
            // Add item
            final DirectItemOrHeader itemOrHeader = new DirectItemOrHeader();
            itemOrHeader.item = item;
            itemOrHeaders.add(itemOrHeader);
            prevSectionDate = DateUtils.dateAtZeroHours(item.getDate());
        }
        return itemOrHeaders;
    }

    public List<DirectItemOrHeader> getList() {
        return differ.getCurrentList();
    }

    public List<DirectItem> getItems() {
        return items;
    }

    @Override
    public void onViewRecycled(@NonNull final RecyclerView.ViewHolder holder) {
        if (holder instanceof DirectItemViewHolder) {
            ((DirectItemViewHolder) holder).cleanup();
        }
    }

    @Override
    public void onViewDetachedFromWindow(@NonNull final RecyclerView.ViewHolder holder) {
        if (holder instanceof DirectItemViewHolder) {
            ((DirectItemViewHolder) holder).cleanup();
        }
    }

    public DirectThread getThread() {
        return thread;
    }

    public static class DirectItemOrHeader {
        Date date;
        public DirectItem item;

        public boolean isHeader() {
            return date != null;
        }

        @NonNull
        @Override
        public String toString() {
            return "DirectItemOrHeader{" +
                    "date=" + date +
                    ", item=" + (item != null ? item.getItemType() : null) +
                    '}';
        }
    }

    public static class HeaderViewHolder extends RecyclerView.ViewHolder {
        private final LayoutDmHeaderBinding binding;

        public HeaderViewHolder(@NonNull final LayoutDmHeaderBinding binding) {
            super(binding.getRoot());
            this.binding = binding;
        }

        public void bind(final Date date) {
            if (date == null) {
                binding.header.setText("");
                return;
            }
            binding.header.setText(DateFormat.getDateFormat(itemView.getContext()).format(date));
        }
    }

    public interface DirectItemCallback {
        void onHashtagClick(String hashtag);

        void onMentionClick(String mention);

        void onLocationClick(long locationId);

        void onURLClick(String url);

        void onEmailClick(String email);

        void onMediaClick(Media media);

        void onStoryClick(DirectItemStoryShare storyShare);

        void onReaction(DirectItem item, Emoji emoji);

        void onReactionClick(DirectItem item, int position);

        void onOptionSelect(DirectItem item, @IdRes int itemId, final Function<DirectItem, Void> callback);

        void onAddReactionListener(DirectItem item);
    }

    public interface DirectItemInternalLongClickListener {
        void onLongClick(int position, DirectItemViewHolder viewHolder);
    }

    public interface DirectItemLongClickListener {
        void onLongClick(int position);
    }
}