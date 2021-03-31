using System;
using System.Diagnostics;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace MelonLoader
{
    public class MelonLogger
    {
        public static void Log(string s)
        {
            string namesection = GetNameSection();
            Native_Log(namesection, s);
            MelonConsole.RunLogCallbacks(namesection, s);
        }

        public static void Log(ConsoleColor color, string s)
        {
            string namesection = GetNameSection();
            Native_LogColor(namesection, s, color);
            MelonConsole.RunLogCallbacks(namesection, s);
        }

        public static void Log(string s, params object[] args)
        {
            string namesection = GetNameSection();
            string fmt = string.Format(s, args);
            Native_Log(namesection, fmt);
            MelonConsole.RunLogCallbacks(namesection, fmt);
        }

        public static void Log(ConsoleColor color, string s, params object[] args)
        {
            string namesection = GetNameSection();
            string fmt = string.Format(s, args);
            Native_LogColor(namesection, fmt, color);
            MelonConsole.RunLogCallbacks(namesection, fmt);
        }

        public static void Log(object o)
        {
            Log(o.ToString());
        }

        public static void Log(ConsoleColor color, object o)
        {
            Log(color, o.ToString());
        }

        public static void LogWarning(string s)
        {
            string namesection = GetNameSection();
            Native_LogWarning(namesection, s);
            MelonConsole.RunWarningCallbacks(namesection, s);
        }

        public static void LogWarning(string s, params object[] args)
        {
            string namesection = GetNameSection();
            string fmt = string.Format(s, args);
            Native_LogWarning(namesection, fmt);
            MelonConsole.RunWarningCallbacks(namesection, fmt);
            Native_LogWarning(GetNameSection(), fmt);
        }

        public static void LogError(string s)
        {
            string namesection = GetNameSection();
            Native_LogError(namesection, s);
            MelonConsole.RunErrorCallbacks(namesection, s);
        }
        public static void LogError(string s, params object[] args)
        {
            string namesection = GetNameSection();
            string fmt = string.Format(s, args);
            Native_LogError(namesection, fmt);
            MelonConsole.RunErrorCallbacks(namesection, fmt);
        }

        internal static void LogMelonError(string msg, string modname)
        {
            string namesection = (string.IsNullOrEmpty(modname) ? "" : ("[" + modname.Replace(" ", "_") + "] "));
            Native_LogMelonError(namesection, msg);
            MelonConsole.RunErrorCallbacks(namesection, msg);
        }

        internal static void LogMelonCompatibility(MelonBase.MelonCompatibility comp) => Native_LogMelonCompatibility(comp);

        internal static string GetNameSection()
        {
            StackTrace st = new StackTrace(2, true);
            StackFrame sf = st.GetFrame(0);
            if (sf != null)
            {
                MethodBase method = sf.GetMethod();
                if (!method.Equals(null))
                {
                    Type methodClassType = method.DeclaringType;
                    if (!methodClassType.Equals(null))
                    {
                        Assembly asm = methodClassType.Assembly;
                        if (!asm.Equals(null))
                        {
                            MelonPlugin plugin = MelonHandler.Plugins.Find(x => (x.Assembly == asm));
                            if (plugin != null)
                            {
                                if (!string.IsNullOrEmpty(plugin.Info.Name))
                                    return "[" + plugin.Info.Name.Replace(" ", "_") + "] ";
                            }
                            else
                            {
                                MelonMod mod = MelonHandler.Mods.Find(x => (x.Assembly == asm));
                                if (mod != null)
                                {
                                    if (!string.IsNullOrEmpty(mod.Info.Name))
                                        return "[" + mod.Info.Name.Replace(" ", "_") + "] ";
                                }
                            }
                        }
                    }
                }
            }
            return "";
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Native_Log(string namesection, string txt);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Native_LogColor(string namesection, string txt, ConsoleColor color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Native_LogWarning(string namesection, string txt);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Native_LogError(string namesection, string txt);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Native_LogMelonError(string namesection, string txt);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Native_LogMelonCompatibility(MelonBase.MelonCompatibility comp);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Native_ThrowInternalError(string txt);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public extern static IntPtr Native_GetConsoleOutputHandle();
    }
}