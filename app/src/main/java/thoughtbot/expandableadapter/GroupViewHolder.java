package thoughtbot.expandableadapter;

import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.R;
import awais.instagrabber.interfaces.OnGroupClickListener;

public class GroupViewHolder extends RecyclerView.ViewHolder implements View.OnClickListener {
    private final OnGroupClickListener listener;
    private final TextView title;
    private final ImageView arrow;

    public GroupViewHolder(@NonNull final View itemView, final OnGroupClickListener listener) {
        super(itemView);
        this.listener = listener;
        this.title = itemView.findViewById(android.R.id.text1);
        this.arrow = itemView.findViewById(R.id.collapsingArrow);
        this.title.setBackgroundColor(0x80_1565C0);
        itemView.setOnClickListener(this);
    }

    public void setTitle(@NonNull final String title) {
        this.title.setText(title);
    }

    @Override
    public void onClick(final View v) {
        if (listener != null) listener.toggleGroup(getLayoutPosition());
    }

    public void toggle(final boolean expand) {
        arrow.setImageResource(expand ? R.drawable.ic_keyboard_arrow_up_24 : R.drawable.ic_keyboard_arrow_down_24);
    }
}