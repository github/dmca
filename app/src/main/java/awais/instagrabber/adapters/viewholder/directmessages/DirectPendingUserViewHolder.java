package awais.instagrabber.adapters.viewholder.directmessages;

import android.graphics.drawable.Drawable;
import android.text.SpannableStringBuilder;
import android.text.Spanned;
import android.util.Log;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.appcompat.content.res.AppCompatResources;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectPendingUsersAdapter.PendingUser;
import awais.instagrabber.adapters.DirectPendingUsersAdapter.PendingUserCallback;
import awais.instagrabber.customviews.VerticalImageSpan;
import awais.instagrabber.databinding.LayoutDmPendingUserItemBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Utils;

public class DirectPendingUserViewHolder extends RecyclerView.ViewHolder {
    private static final String TAG = DirectPendingUserViewHolder.class.getSimpleName();

    private final LayoutDmPendingUserItemBinding binding;
    private final PendingUserCallback callback;
    private final int drawableSize;

    private VerticalImageSpan verifiedSpan;

    public DirectPendingUserViewHolder(@NonNull final LayoutDmPendingUserItemBinding binding,
                                       final PendingUserCallback callback) {
        super(binding.getRoot());
        this.binding = binding;
        this.callback = callback;
        drawableSize = Utils.convertDpToPx(24);
    }

    public void bind(final int position, final PendingUser pendingUser) {
        if (pendingUser == null) return;
        binding.getRoot().setOnClickListener(v -> {
            if (callback == null) return;
            callback.onClick(position, pendingUser);
        });
        setUsername(pendingUser);
        binding.requester.setText(itemView.getResources().getString(R.string.added_by, pendingUser.getRequester()));
        binding.profilePic.setImageURI(pendingUser.getUser().getProfilePicUrl());
        if (pendingUser.isInProgress()) {
            binding.approve.setVisibility(View.GONE);
            binding.deny.setVisibility(View.GONE);
            binding.progress.setVisibility(View.VISIBLE);
            return;
        }
        binding.approve.setVisibility(View.VISIBLE);
        binding.deny.setVisibility(View.VISIBLE);
        binding.progress.setVisibility(View.GONE);
        binding.approve.setOnClickListener(v -> {
            if (callback == null) return;
            callback.onApprove(position, pendingUser);
        });
        binding.deny.setOnClickListener(v -> {
            if (callback == null) return;
            callback.onDeny(position, pendingUser);
        });
    }

    private void setUsername(final PendingUser pendingUser) {
        final User user = pendingUser.getUser();
        final SpannableStringBuilder sb = new SpannableStringBuilder(user.getUsername());
        if (user.isVerified()) {
            if (verifiedSpan == null) {
                final Drawable verifiedDrawable = AppCompatResources.getDrawable(itemView.getContext(), R.drawable.verified);
                if (verifiedDrawable != null) {
                    final Drawable drawable = verifiedDrawable.mutate();
                    drawable.setBounds(0, 0, drawableSize, drawableSize);
                    verifiedSpan = new VerticalImageSpan(drawable);
                }
            }
            try {
                if (verifiedSpan != null) {
                    sb.append("  ");
                    sb.setSpan(verifiedSpan, sb.length() - 1, sb.length(), Spanned.SPAN_EXCLUSIVE_EXCLUSIVE);
                }
            } catch (Exception e) {
                Log.e(TAG, "bind: ", e);
            }
        }
        binding.username.setText(sb);
    }
}
