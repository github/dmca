package awais.instagrabber.adapters.viewholder;

import android.view.View;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import awais.instagrabber.adapters.SliderItemsAdapter;
import awais.instagrabber.repositories.responses.Media;

public abstract class SliderItemViewHolder extends RecyclerView.ViewHolder {
    private static final String TAG = "FeedSliderItemViewHolder";

    public SliderItemViewHolder(@NonNull final View itemView) {
        super(itemView);
    }

    public abstract void bind(final Media media,
                              final int position,
                              final SliderItemsAdapter.SliderCallback sliderCallback);
}
