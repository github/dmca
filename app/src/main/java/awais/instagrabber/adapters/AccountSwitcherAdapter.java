package awais.instagrabber.adapters;

import android.annotation.SuppressLint;
import android.graphics.Typeface;
import android.text.TextUtils;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.R;
import awais.instagrabber.databinding.PrefAccountSwitcherBinding;
import awais.instagrabber.db.entities.Account;
import awais.instagrabber.utils.Constants;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class AccountSwitcherAdapter extends ListAdapter<Account, AccountSwitcherAdapter.ViewHolder> {
    private static final String TAG = "AccountSwitcherAdapter";
    private static final DiffUtil.ItemCallback<Account> DIFF_CALLBACK = new DiffUtil.ItemCallback<Account>() {
        @Override
        public boolean areItemsTheSame(@NonNull final Account oldItem, @NonNull final Account newItem) {
            return oldItem.getUid().equals(newItem.getUid());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final Account oldItem, @NonNull final Account newItem) {
            return oldItem.getUid().equals(newItem.getUid());
        }
    };

    private final OnAccountClickListener clickListener;
    private final OnAccountLongClickListener longClickListener;

    public AccountSwitcherAdapter(final OnAccountClickListener clickListener,
                                  final OnAccountLongClickListener longClickListener) {
        super(DIFF_CALLBACK);
        this.clickListener = clickListener;
        this.longClickListener = longClickListener;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater layoutInflater = LayoutInflater.from(parent.getContext());
        final PrefAccountSwitcherBinding binding = PrefAccountSwitcherBinding.inflate(layoutInflater, parent, false);
        return new ViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull final ViewHolder holder, final int position) {
        final Account model = getItem(position);
        if (model == null) return;
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        final boolean isCurrent = model.getCookie().equals(cookie);
        holder.bind(model, isCurrent, clickListener, longClickListener);
    }

    public interface OnAccountClickListener {
        void onAccountClick(final Account model, final boolean isCurrent);
    }

    public interface OnAccountLongClickListener {
        boolean onAccountLongClick(final Account model, final boolean isCurrent);
    }

    public static class ViewHolder extends RecyclerView.ViewHolder {
        private final PrefAccountSwitcherBinding binding;

        public ViewHolder(final PrefAccountSwitcherBinding binding) {
            super(binding.getRoot());
            this.binding = binding;
            binding.arrowDown.setImageResource(R.drawable.ic_check_24);
        }

        @SuppressLint("SetTextI18n")
        public void bind(final Account model,
                         final boolean isCurrent,
                         final OnAccountClickListener clickListener,
                         final OnAccountLongClickListener longClickListener) {
            // Log.d(TAG, model.getFullName());
            itemView.setOnClickListener(v -> {
                if (clickListener == null) return;
                clickListener.onAccountClick(model, isCurrent);
            });
            itemView.setOnLongClickListener(v -> {
                if (longClickListener == null) return false;
                return longClickListener.onAccountLongClick(model, isCurrent);
            });
            binding.profilePic.setImageURI(model.getProfilePic());
            binding.username.setText("@" + model.getUsername());
            binding.fullName.setTypeface(null);
            final String fullName = model.getFullName();
            if (TextUtils.isEmpty(fullName)) {
                binding.fullName.setVisibility(View.GONE);
            } else {
                binding.fullName.setVisibility(View.VISIBLE);
                binding.fullName.setText(fullName);
            }
            if (!isCurrent) {
                binding.arrowDown.setVisibility(View.GONE);
                return;
            }
            binding.fullName.setTypeface(binding.fullName.getTypeface(), Typeface.BOLD);
            binding.arrowDown.setVisibility(View.VISIBLE);
        }
    }
}
