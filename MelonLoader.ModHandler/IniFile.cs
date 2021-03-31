using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace MelonLoader
{
    public class IniFile
    {
        [DllImport("KERNEL32.DLL", EntryPoint = "GetPrivateProfileStringW", SetLastError = true, CharSet = CharSet.Unicode, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        private static extern int GetPrivateProfileString(string lpSection, string lpKey, string lpDefault, StringBuilder lpReturnString, int nSize, string lpFileName);
        [DllImport("KERNEL32.DLL", EntryPoint = "WritePrivateProfileStringW", SetLastError = true, CharSet = CharSet.Unicode, ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        private static extern int WritePrivateProfileString(string lpSection, string lpKey, string lpValue, string lpFileName);
        private string _path = "";
        public string Path { get { return _path; } internal set { if (!File.Exists(value)) File.WriteAllText(value, "", Encoding.Unicode); _path = value; } }
        public IniFile(string INIPath) { Path = INIPath; }
        private void IniWriteValue(string Section, string Key, string Value) { WritePrivateProfileString(Section, Key, Value, Path); }
        private string IniReadValue(string Section, string Key)
        {
            const int MAX_CHARS = 1023;
            StringBuilder result = new StringBuilder(MAX_CHARS);
            GetPrivateProfileString(Section, Key, " _", result, MAX_CHARS, Path);
            if (result.ToString().Equals(" _")) return null;
            return result.ToString();
        }

        public bool HasKey(string section, string name) { return IniReadValue(section, name) != null; }

        public string GetString(string section, string name, string defaultValue = "", bool autoSave = false)
        {
            string value = IniReadValue(section, name);
            if (!string.IsNullOrEmpty(value))
                return value;
            else if (autoSave)
                SetString(section, name, defaultValue);
            return defaultValue;
        }
        public void SetString(string section, string name, string value) { IniWriteValue(section, name, value.Trim()); }

        public int GetInt(string section, string name, int defaultValue = 0, bool autoSave = false)
        {
            int value;
            if (int.TryParse(IniReadValue(section, name), out value))
                return value;
            else if (autoSave)
                SetInt(section, name, defaultValue);
            return defaultValue;
        }
        public void SetInt(string section, string name, int value) { IniWriteValue(section, name, value.ToString()); }

        public float GetFloat(string section, string name, float defaultValue = 0f, bool autoSave = false)
        {
            float value;
            if (float.TryParse(IniReadValue(section, name), out value))
                return value;
            else if (autoSave)
                SetFloat(section, name, defaultValue);
            return defaultValue;
        }
        public void SetFloat(string section, string name, float value) { IniWriteValue(section, name, value.ToString()); }

        public bool GetBool(string section, string name, bool defaultValue = false, bool autoSave = false)
        {
            string sVal = GetString(section, name, null);
            if ("true".Equals(sVal) || "1".Equals(sVal) || "0".Equals(sVal) || "false".Equals(sVal))
                return ("true".Equals(sVal) || "1".Equals(sVal));
            else if (autoSave)
                SetBool(section, name, defaultValue);
            return defaultValue;
        }
        public void SetBool(string section, string name, bool value) { IniWriteValue(section, name, value ? "true" : "false"); }
    }
}
