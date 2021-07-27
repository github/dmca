package awais.instagrabber.services;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;

import java.util.Objects;

import awais.instagrabber.fragments.settings.PreferenceKeys;
import awais.instagrabber.utils.Constants;
import awais.instagrabber.utils.CookieUtils;
import awais.instagrabber.utils.TextUtils;

import static awais.instagrabber.utils.Utils.settingsHelper;

public class BootCompletedReceiver extends BroadcastReceiver {
    @Override
    public void onReceive(final Context context, final Intent intent) {
        if (!Objects.equals(intent.getAction(), "android.intent.action.BOOT_COMPLETED")) return;
        final boolean enabled = settingsHelper.getBoolean(PreferenceKeys.PREF_ENABLE_DM_AUTO_REFRESH);
        if (!enabled) return;
        final String cookie = settingsHelper.getString(Constants.COOKIE);
        final boolean isLoggedIn = !TextUtils.isEmpty(cookie) && CookieUtils.getUserIdFromCookie(cookie) != 0;
        if (!isLoggedIn) return;
        DMSyncAlarmReceiver.setAlarm(context);
    }
}
