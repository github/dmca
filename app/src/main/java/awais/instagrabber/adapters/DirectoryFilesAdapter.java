package awais.instagrabber.adapters;

import android.view.LayoutInflater;
import android.view.ViewGroup;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.DiffUtil;
import androidx.recyclerview.widget.ListAdapter;
import androidx.recyclerview.widget.RecyclerView;

import java.io.File;

import awais.instagrabber.R;
import awais.instagrabber.databinding.ItemDirListBinding;

public final class DirectoryFilesAdapter extends ListAdapter<File, DirectoryFilesAdapter.ViewHolder> {
    private final OnFileClickListener onFileClickListener;

    private static final DiffUtil.ItemCallback<File> DIFF_CALLBACK = new DiffUtil.ItemCallback<File>() {
        @Override
        public boolean areItemsTheSame(@NonNull final File oldItem, @NonNull final File newItem) {
            return oldItem.getAbsolutePath().equals(newItem.getAbsolutePath());
        }

        @Override
        public boolean areContentsTheSame(@NonNull final File oldItem, @NonNull final File newItem) {
            return oldItem.getAbsolutePath().equals(newItem.getAbsolutePath());
        }
    };

    public DirectoryFilesAdapter(final OnFileClickListener onFileClickListener) {
        super(DIFF_CALLBACK);
        this.onFileClickListener = onFileClickListener;
    }

    @NonNull
    @Override
    public ViewHolder onCreateViewHolder(@NonNull final ViewGroup parent, final int viewType) {
        final LayoutInflater inflater = LayoutInflater.from(parent.getContext());
        final ItemDirListBinding binding = ItemDirListBinding.inflate(inflater, parent, false);
        return new ViewHolder(binding);
    }

    @Override
    public void onBindViewHolder(@NonNull final ViewHolder holder, final int position) {
        final File file = getItem(position);
        holder.bind(file, onFileClickListener);
    }

    public interface OnFileClickListener {
        void onFileClick(File file);
    }

    static final class ViewHolder extends RecyclerView.ViewHolder {
        private final ItemDirListBinding binding;

        private ViewHolder(final ItemDirListBinding binding) {
            super(binding.getRoot());
            this.binding = binding;
        }

        public void bind(final File file, final OnFileClickListener onFileClickListener) {
            if (file == null) return;
            if (onFileClickListener != null) {
                itemView.setOnClickListener(v -> onFileClickListener.onFileClick(file));
            }
            binding.text.setText(file.getName());
            if (file.isDirectory()) {
                binding.icon.setImageResource(R.drawable.ic_folder_24);
                return;
            }
            binding.icon.setImageResource(R.drawable.ic_file_24);
        }
    }
}
