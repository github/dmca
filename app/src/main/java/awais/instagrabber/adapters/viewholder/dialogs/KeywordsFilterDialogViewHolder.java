package awais.instagrabber.adapters.viewholder.dialogs;

import android.content.Context;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;
import java.util.HashSet;

import awais.instagrabber.R;
import awais.instagrabber.adapters.KeywordsFilterAdapter;
import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.utils.SettingsHelper;

public class KeywordsFilterDialogViewHolder extends RecyclerView.ViewHolder {

    private final Button deleteButton;
    private final TextView item;

    public KeywordsFilterDialogViewHolder(@NonNull View itemView) {
        super(itemView);
        deleteButton = itemView.findViewById(R.id.keyword_delete);
        item = itemView.findViewById(R.id.keyword_text);
    }

    public void bind(ArrayList<String> items, int position, Context context, KeywordsFilterAdapter adapter){
        item.setText(items.get(position));
        deleteButton.setOnClickListener(view -> {
            final String s = items.get(position);
            SettingsHelper settingsHelper = new SettingsHelper(context);
            items.remove(position);
            settingsHelper.putStringSet(PreferenceKeys.KEYWORD_FILTERS, new HashSet<>(items));
            adapter.notifyDataSetChanged();
            final String message = context.getString(R.string.removed_keywords, s);
            Toast.makeText(context, message, Toast.LENGTH_SHORT).show();
        });
    }

    public Button getDeleteButton(){
        return deleteButton;
    }

    public TextView getTextView(){
        return item;
    }
}
