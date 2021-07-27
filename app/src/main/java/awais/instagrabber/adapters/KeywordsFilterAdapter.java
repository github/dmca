package awais.instagrabber.adapters;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.util.ArrayList;

import awais.instagrabber.R;
import awais.instagrabber.adapters.viewholder.dialogs.KeywordsFilterDialogViewHolder;

public class KeywordsFilterAdapter extends RecyclerView.Adapter<KeywordsFilterDialogViewHolder> {

    private final Context context;
    private final ArrayList<String> items;

    public KeywordsFilterAdapter(Context context, ArrayList<String> items){
        this.context = context;
        this.items = items;
    }

    @NonNull
    @Override
    public KeywordsFilterDialogViewHolder onCreateViewHolder(@NonNull ViewGroup parent, int viewType) {
        final View v = LayoutInflater.from(parent.getContext()).inflate(R.layout.item_keyword, parent, false);
        return new KeywordsFilterDialogViewHolder(v);
    }

    @Override
    public void onBindViewHolder(@NonNull KeywordsFilterDialogViewHolder holder, int position) {
        holder.bind(items, position, context, this);
    }

    @Override
    public int getItemCount() {
        return items.size();
    }
}
