package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.fragment.app.DialogFragment;
import androidx.recyclerview.widget.LinearLayoutManager;

import java.util.Collections;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.AccountSwitcherAdapter;
import awais.instagrabber.databinding.DialogAccountSwitcherBinding;
import awais.instagrabber.db.datasources.AccountDataSource;
import awais.instagrabber.db.entities.Account;
import awais.instagrabber.db.repositories.AccountRepository;
import awais.instagrabber.db.repositories.RepositoryCallback;
import awais.instagrabber.utils.AppExecutors;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.ProcessPhoenix;
import awais.instagrabber.utils.TextUtils;
import awais.instagrabber.utils.Utils;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class AccountSwitcherDialogFragment extends DialogFragment {

    private AccountRepository accountRepository;

    private OnAddAccountClickListener onAddAccountClickListener;
    private DialogAccountSwitcherBinding binding;

    public AccountSwitcherDialogFragment() {
        accountRepository = AccountRepository.getInstance(AccountDataSource.getInstance(getContext()));
    }

    public AccountSwitcherDialogFragment(final OnAddAccountClickListener onAddAccountClickListener) {
        this.onAddAccountClickListener = onAddAccountClickListener;
        accountRepository = AccountRepository.getInstance(AccountDataSource.getInstance(getContext()));
    }

    private final AccountSwitcherAdapter.OnAccountClickListener accountClickListener = (model, isCurrent) -> {
        if (isCurrent) {
            dismiss();
            return;
        }
        CookieUtils.setupCookies(model.getCookie());
        settingsHelper.putString(Constants.COOKIE, model.getCookie());
        // final FragmentActivity activity = getActivity();
        // if (activity != null) activity.recreate();
        // dismiss();
        AppExecutors.getInstance().mainThread().execute(() -> {
            final Context context = getContext();
            if (context == null) return;
            ProcessPhoenix.triggerRebirth(context);
        }, 200);
    };

    private final AccountSwitcherAdapter.OnAccountLongClickListener accountLongClickListener = (model, isCurrent) -> {
        final Context context = getContext();
        if (context == null) return false;
        if (isCurrent) {
            new AlertDialog.Builder(context)
                    .setMessage(R.string.quick_access_cannot_delete_curr)
                    .setPositiveButton(R.string.ok, null)
                    .show();
            return true;
        }
        new AlertDialog.Builder(context)
                .setMessage(getString(R.string.quick_access_confirm_delete, model.getUsername()))
                .setPositiveButton(R.string.yes, (dialog, which) -> {
                    if (accountRepository == null) return;
                    accountRepository.deleteAccount(model, new RepositoryCallback<Void>() {
                        @Override
                        public void onSuccess(final Void result) {
                            dismiss();
                        }

                        @Override
                        public void onDataNotAvailable() {
                            dismiss();
                        }
                    });
                })
                .setNegativeButton(R.string.cancel, null)
                .show();
        dismiss();
        return true;
    };

    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater,
                             final ViewGroup container,
                             final Bundle savedInstanceState) {
        binding = DialogAccountSwitcherBinding.inflate(inflater, container, false);
        binding.accounts.setLayoutManager(new LinearLayoutManager(getContext()));
        return binding.getRoot();
    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        init();
    }

    @Override
    public void onStart() {
        super.onStart();
        final Dialog dialog = getDialog();
        if (dialog == null) return;
        final Window window = dialog.getWindow();
        if (window == null) return;
        final int height = ViewGroup.LayoutParams.WRAP_CONTENT;
        final int width = (int) (Utils.displayMetrics.widthPixels * 0.8);
        window.setLayout(width, height);
    }

    private void init() {
        final AccountSwitcherAdapter adapter = new AccountSwitcherAdapter(accountClickListener, accountLongClickListener);
        binding.accounts.setAdapter(adapter);
        if (accountRepository == null) return;
        accountRepository.getAllAccounts(new RepositoryCallback<List<Account>>() {
            @Override
            public void onSuccess(final List<Account> accounts) {
                if (accounts == null) return;
                final String cookie = settingsHelper.getString(Constants.COOKIE);
                sortUserList(cookie, accounts);
                adapter.submitList(accounts);
            }

            @Override
            public void onDataNotAvailable() {}
        });
        binding.addAccountBtn.setOnClickListener(v -> {
            if (onAddAccountClickListener == null) return;
            onAddAccountClickListener.onAddAccountClick(this);
        });
    }

    /**
     * Sort the user list by following logic:
     * <ol>
     * <li>Keep currently active account at top.
     * <li>Check if any user does not have a full name.
     * <li>If all have full names, sort by full names.
     * <li>Otherwise, sort by the usernames
     * </ol>
     *
     * @param cookie   active cookie
     * @param allUsers list of users
     */
    private void sortUserList(final String cookie, final List<Account> allUsers) {
        boolean sortByName = true;
        for (final Account user : allUsers) {
            if (TextUtils.isEmpty(user.getFullName())) {
                sortByName = false;
                break;
            }
        }
        final boolean finalSortByName = sortByName;
        Collections.sort(allUsers, (o1, o2) -> {
            // keep current account at top
            if (o1.getCookie().equals(cookie)) return -1;
            if (finalSortByName) {
                // sort by full name
                return o1.getFullName().compareTo(o2.getFullName());
            }
            // otherwise sort by username
            return o1.getUsername().compareTo(o2.getUsername());
        });
    }

    public interface OnAddAccountClickListener {
        void onAddAccountClick(final AccountSwitcherDialogFragment dialogFragment);
    }
}
