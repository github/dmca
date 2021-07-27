package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

import awais.instagrabber.adapters.viewholder.NotificationViewHolder;
import awais.instagrabber.databinding.ItemNotificationBinding;
import awais.instagrabber.models.enums.NotificationType;
import awais.instagrabber.repositories.responses.notification.Notification;

public final class NotificationsAdapter extends ListAdapter<Notification, NotificationViewHolder> {
    private final OnNotificationClickListener notificationClickListener;

    private static final DiffUtil.ItemCallback<Notification> DIFF_CALLBACK = new DiffUtil.ItemCallback<Notification>() {
        @Override
        public boolean areItemsTheSame(final Notification oldItem, final Notification newItem) {
            return oldItem != null && newItem != null && oldItem.getPk().equals(newItem.getPk());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final Notification oldItem, @NonNull final Notification newItem) {
            return oldItem.getPk().equals(newItem.getPk());
        }
    };

    public NotificationsAdapter(final OnNotificationClickListener notificationClickListener) {
        super(DIFF_CALLBACK);
        this.notificationClickListener = notificationClickListener;
    }

    @NonNull
    @Override
    public NotificationViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int type) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final ItemNotificationBinding binding = ItemNotificationBinding.inflate(layoutInflater, parent, false);
        return new NotificationViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull final NotificationViewHolder holder, final int position) {
        final Notification Notification = getItem(position);
        holder.bind(Notification, notificationClickListener);
    }

    @Override
    public void submitList(@Nullable final List<Notification> list, @Nullable final Runnable commitCallback) {
        if (list == null) {
            super.submitList(null, commitCallback);
            return;
        }
        super.submitList(sort(list), commitCallback);
    }

    @Override
    public void submitList(@Nullable final List<Notification> list) {
        if (list == null) {
            super.submitList(null);
            return;
        }
        super.submitList(sort(list));
    }

    private List<Notification> sort(final List<Notification> list) {
        final List<Notification> listCopy = new ArrayList<>(list).stream()
                .filter(i -> i.getType() != null)
                .collect(Collectors.toList());
        Collections.sort(listCopy, (o1, o2) -> {
            // keep requests at top
            if (o1.getType() == o2.getType()
                    && o1.getType() == NotificationType.REQUEST
                    && o2.getType() == NotificationType.REQUEST) return 0;
            else if (o1.getType() == NotificationType.REQUEST) return -1;
            else if (o2.getType() == NotificationType.REQUEST) return 1;
            // timestamp
            return Double.compare(o2.getArgs().getTimestamp(), o1.getArgs().getTimestamp());
        });
        return listCopy;
    }

    public interface OnNotificationClickListener {
        void onNotificationClick(final Notification model);

        void onProfileClick(final String username);

        void onPreviewClick(final Notification model);
    }
}