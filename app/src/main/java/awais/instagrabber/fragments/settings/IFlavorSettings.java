package awais.instagrabber.fragments.settings;

import android.content.Context;

import androidx.fragment.app.FragmentManager;
import androidx.preference.Preference;

import java.util.List;

public interface IFlavorSettings {
    List<Preference> getPreferences(Context context,
                                    FragmentManager childFragmentManager,
                                    SettingCategory settingCategory);
}
