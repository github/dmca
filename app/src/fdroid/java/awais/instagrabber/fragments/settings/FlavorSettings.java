package awais.instagrabber.fragments.settings;

import android.content.Context;

import androidx.annotation.NonNull;
import androidx.fragment.app.FragmentManager;
import androidx.preference.Preference;

import java.util.Collections;
import java.util.List;

import awais.instagrabber.fragments.settings.IFlavorSettings;
import awais.instagrabber.fragments.settings.SettingCategory;

public final class FlavorSettings implements IFlavorSettings {

    private static FlavorSettings instance;

    private FlavorSettings() {
    }

    public static FlavorSettings getInstance() {
        if (instance == null) {
            instance = new FlavorSettings();
        }
        return instance;
    }

    @NonNull
    @Override
    public List<Preference> getPreferences(@NonNull final Context context,
                                           @NonNull final FragmentManager fragmentManager,
                                           @NonNull final SettingCategory settingCategory) {
        // switch (settingCategory) {
        //     default:
        //         break;
        // }
        return Collections.emptyList();
    }
}
