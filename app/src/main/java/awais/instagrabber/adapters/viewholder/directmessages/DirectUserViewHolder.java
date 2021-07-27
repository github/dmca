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
import awais.instagrabber.adapters.DirectUsersAdapter.OnDirectUserClickListener;
import awais.instagrabber.adapters.DirectUsersAdapter.OnDirectUserLongClickListener;
import awais.instagrabber.customviews.VerticalImageSpan;
import awais.instagrabber.databinding.LayoutDmUserItemBinding;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.utils.Utils;

public class DirectUserViewHolder extends RecyclerView.ViewHolder {
    private static final String TAG = DirectUserViewHolder.class.getSimpleName();

    private final LayoutDmUserItemBinding binding;
    private final OnDirectUserClickListener onClickListener;
    private final OnDirectUserLongClickListener onLongClickListener;
    private final int drawableSize;

    private VerticalImageSpan verifiedSpan;

    public DirectUserViewHolder(@NonNull final LayoutDmUserItemBinding binding,
                                final OnDirectUserClickListener onClickListener,
                                final OnDirectUserLongClickListener onLongClickListener) {
        super(binding.getRoot());
        this.binding = binding;
        this.onClickListener = onClickListener;
        this.onLongClickListener = onLongClickListener;
        drawableSize = Utils.convertDpToPx(24);
    }

    public void bind(final int position,
                     final User user,
                     final boolean isAdmin,
                     final boolean isInviter,
                     final boolean showSelection,
                     final boolean isSelected) {
        if (user == null) return;
        binding.getRoot().setOnClickListener(v -> {
            if (onClickListener == null) return;
            onClickListener.onClick(position, user, isSelected);
        });
        binding.getRoot().setOnLongClickListener(v -> {
            if (onLongClickListener == null) return false;
            return onLongClickListener.onLongClick(position, user);
        });
        setFullName(user);
        binding.username.setText(user.getUsername());
        binding.profilePic.setImageURI(user.getProfilePicUrl());
        setInfo(isAdmin, isInviter);
        setSelection(showSelection, isSelected);
    }

    private void setFullName(final User user) {
        final SpannableStringBuilder sb = new SpannableStringBuilder(user.getFullName());
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
        binding.fullName.setText(sb);
    }

    private void setInfo(final boolean isAdmin, final boolean isInviter) {
        if (!isAdmin && !isInviter) {
            binding.info.setVisibility(View.GONE);
            return;
        }
        if (isAdmin) {
            binding.info.setText(R.string.admin);
            return;
        }
        binding.info.setText(R.string.inviter);
    }

    private void setSelection(final boolean showSelection, final boolean isSelected) {
        binding.select.setVisibility(showSelection ? View.VISIBLE : View.GONE);
        binding.getRoot().setSelected(isSelected);
    }
}
