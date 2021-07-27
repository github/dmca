package awais.instagrabber.broadcasts;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

public class DMRefreshBroadcastReceiver extends BroadcastReceiver {
    public static final String ACTION_REFRESH_DM = "action_refresh_dm";
    private final OnDMRefreshCallback callback;

    public DMRefreshBroadcastReceiver(final OnDMRefreshCallback callback) {
        this.callback = callback;
    }

    @Override
    public void onReceive(final Context context, final Intent intent) {
        if (callback == null) return;
        final String action = intent.getAction();
        if (action == null) return;
        if (!action.equals(ACTION_REFRESH_DM)) return;
        callback.onReceive();
    }

    public interface OnDMRefreshCallback {
        void onReceive();
    }
}
