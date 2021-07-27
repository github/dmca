package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import com.google.android.material.bottomsheet.BottomSheetDialog;
import com.google.android.material.bottomsheet.BottomSheetDialogFragment;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

import awais.instagrabber.R;
import awais.instagrabber.adapters.DirectReactionsAdapter;
import awais.instagrabber.adapters.DirectReactionsAdapter.OnReactionClickListener;
import awais.instagrabber.repositories.responses.User;
import awais.instagrabber.repositories.responses.directmessages.DirectItemReactions;
import awais.instagrabber.utils.TextUtils;

public class DirectItemReactionDialogFragment extends BottomSheetDialogFragment {

    private static final String ARG_VIEWER_ID = "viewerId";
    private static final String ARG_ITEM_ID = "itemId";
    private static final String ARG_USERS = "users";
    private static final String ARG_REACTIONS = "reactions";

    private RecyclerView recyclerView;
    private OnReactionClickListener onReactionClickListener;

    public static DirectItemReactionDialogFragment newInstance(final long viewerId,
                                                               @NonNull final ArrayList<User> users,
                                                               @NonNull final String itemId,
                                                               @NonNull final DirectItemReactions reactions) {
        Bundle args = new Bundle();
        args.putLong(ARG_VIEWER_ID, viewerId);
        args.putSerializable(ARG_USERS, users);
        args.putString(ARG_ITEM_ID, itemId);
        args.putSerializable(ARG_REACTIONS, reactions);
        DirectItemReactionDialogFragment fragment = new DirectItemReactionDialogFragment();
        fragment.setArguments(args);
        return fragment;
    }

    public DirectItemReactionDialogFragment() {}

    @Override
    public void onCreate(@Nullable final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setStyle(DialogFragment.STYLE_NORMAL, R.style.ThemeOverlay_Rounded_BottomSheetDialog);
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull final LayoutInflater inflater, @Nullable final ViewGroup container, @Nullable final Bundle savedInstanceState) {
        final Context context = getContext();
        if (context == null) {
            return null;
        }
        recyclerView = new RecyclerView(context);
        return recyclerView;

    }

    @Override
    public void onViewCreated(@NonNull final View view, @Nullable final Bundle savedInstanceState) {
        init();
    }

    @Override
    public void onAttach(@NonNull final Context context) {
        super.onAttach(context);
        try {
            onReactionClickListener = (OnReactionClickListener) getParentFragment();
        } catch (ClassCastException e) {
            throw new ClassCastException("Calling fragment must implement DirectReactionsAdapter.OnReactionClickListener interface");
        }
    }

    @Override
    public void onStart() {
        super.onStart();
        final Dialog dialog = getDialog();
        if (dialog == null) return;
        final BottomSheetDialog bottomSheetDialog = (BottomSheetDialog) dialog;
        final View bottomSheetInternal = bottomSheetDialog.findViewById(com.google.android.material.R.id.design_bottom_sheet);
        if (bottomSheetInternal == null) return;
        bottomSheetInternal.getLayoutParams().height = ViewGroup.LayoutParams.MATCH_PARENT;
        bottomSheetInternal.requestLayout();
    }

    private void init() {
        final Context context = getContext();
        if (context == null) return;
        final Bundle arguments = getArguments();
        if (arguments == null) return;
        final long viewerId = arguments.getLong(ARG_VIEWER_ID);
        final Serializable usersSerializable = arguments.getSerializable(ARG_USERS);
        if (!(usersSerializable instanceof ArrayList)) return;
        //noinspection unchecked
        final List<User> users = (ArrayList<User>) usersSerializable;
        final Serializable reactionsSerializable = arguments.getSerializable(ARG_REACTIONS);
        if (!(reactionsSerializable instanceof DirectItemReactions)) return;
        final DirectItemReactions reactions = (DirectItemReactions) reactionsSerializable;
        final String itemId = arguments.getString(ARG_ITEM_ID);
        if (TextUtils.isEmpty(itemId)) return;
        recyclerView.setLayoutManager(new LinearLayoutManager(context));
        final DirectReactionsAdapter adapter = new DirectReactionsAdapter(viewerId, users, itemId, onReactionClickListener);
        recyclerView.setAdapter(adapter);
        adapter.submitList(reactions.getEmojis());
    }
}
