package awais.instagrabber.customviews;

import android.view.ActionMode;
import android.view.Menu;
import android.view.MenuItem;

public class PrimaryActionModeCallback implements ActionMode.Callback {
    private ActionMode mode;
    private int menuRes;
    private final Callbacks callbacks;

    public PrimaryActionModeCallback(final int menuRes, final Callbacks callbacks) {
        this.menuRes = menuRes;
        this.callbacks = callbacks;
    }

    @Override
    public boolean onCreateActionMode(final ActionMode mode, final Menu menu) {
        this.mode = mode;
        mode.getMenuInflater().inflate(menuRes, menu);
        if (callbacks != null) {
            callbacks.onCreate(mode, menu);
        }
        return true;
    }

    @Override
    public boolean onPrepareActionMode(final ActionMode mode, final Menu menu) {
        return false;
    }

    @Override
    public boolean onActionItemClicked(final ActionMode mode, final MenuItem item) {
        if (callbacks != null) {
            return callbacks.onActionItemClicked(mode, item);
        }
        return false;
    }

    @Override
    public void onDestroyActionMode(final ActionMode mode) {
        if (callbacks != null) {
            callbacks.onDestroy(mode);
        }
        this.mode = null;
    }

    public abstract static class CallbacksHelper implements Callbacks {
        public void onCreate(final ActionMode mode, final Menu menu) {

        }

        @Override
        public void onDestroy(final ActionMode mode) {

        }

        @Override
        public boolean onActionItemClicked(final ActionMode mode, final MenuItem item) {
            return false;
        }
    }

    public interface Callbacks {
        void onCreate(final ActionMode mode, final Menu menu);

        void onDestroy(final ActionMode mode);

        boolean onActionItemClicked(final ActionMode mode, final MenuItem item);
    }
}
