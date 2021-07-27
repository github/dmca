package awais.instagrabber.dialogs;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.widget.EditText;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.DialogFragment;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.HashSet;

import awais.instagrabber.R;
import awais.instagrabber.adapters.KeywordsFilterAdapter;
import awais.instagrabber.databinding.DialogKeywordsFilterBinding;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.utils.SettingsHelper;
import awais.instagrabber.utils.Utils;

public final class KeywordsFilterDialog extends DialogFragment {

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

    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        final DialogKeywordsFilterBinding dialogKeywordsFilterBinding = DialogKeywordsFilterBinding.inflate(inflater, container, false);
        init(dialogKeywordsFilterBinding, getContext());
        dialogKeywordsFilterBinding.btnOK.setOnClickListener(view -> this.dismiss());
        return dialogKeywordsFilterBinding.getRoot();
    }

    private void init(DialogKeywordsFilterBinding dialogKeywordsFilterBinding, Context context){
        final LinearLayoutManager linearLayoutManager = new LinearLayoutManager(context);
        final RecyclerView recyclerView = dialogKeywordsFilterBinding.recyclerKeyword;
        recyclerView.setLayoutManager(linearLayoutManager);

        final SettingsHelper settingsHelper = new SettingsHelper(context);
        final ArrayList<String> items = new ArrayList<>(settingsHelper.getStringSet(PreferenceKeys.KEYWORD_FILTERS));
        final KeywordsFilterAdapter adapter = new KeywordsFilterAdapter(context, items);
        recyclerView.setAdapter(adapter);

        final EditText editText = dialogKeywordsFilterBinding.editText;

        dialogKeywordsFilterBinding.btnAdd.setOnClickListener(view ->{
            final String s = editText.getText().toString();
            if(s.isEmpty()) return;
            if(items.contains(s)) {
                editText.setText("");
                return;
            }
            items.add(s.toLowerCase());
            settingsHelper.putStringSet(PreferenceKeys.KEYWORD_FILTERS, new HashSet<>(items));
            adapter.notifyItemInserted(items.size());
            final String message = context.getString(R.string.added_keywords, s);
            Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
            editText.setText("");
        });
    }
}
