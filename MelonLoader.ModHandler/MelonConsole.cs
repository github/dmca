using System;
using System.IO;
using System.Runtime.CompilerServices;
using Microsoft.Win32.SafeHandles;

namespace MelonLoader
{
    public class MelonConsole
    {
        public static bool Enabled { get; internal set; }

        internal static void Check()
        {
            if (!Imports.IsDebugMode()
#if !DEBUG
                && IsConsoleEnabled()
#endif
                )
                Create();
        }

        private static void Create()
        {
            Enabled = true;
            Allocate();
            Console.SetOut(new StreamWriter(new FileStream(new SafeFileHandle(MelonLogger.Native_GetConsoleOutputHandle(), false), FileAccess.Write)) { AutoFlush = true });
            Console.SetIn(new StreamReader(Console.OpenStandardInput()));
            SetTitle(BuildInfo.Name + " v" + BuildInfo.Version + " Open-Beta");
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private extern static bool IsConsoleEnabled();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Allocate();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTitle(string title);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetColor(ConsoleColor color);

        internal static void RunLogCallbacks(string namesection, string msg) => LogCallbackHandler?.Invoke(namesection, msg);
        public static event Action<string, string> LogCallbackHandler;
        internal static void RunWarningCallbacks(string namesection, string msg) => WarningCallbackHandler?.Invoke(namesection, msg);
        public static event Action<string, string> WarningCallbackHandler;
        internal static void RunErrorCallbacks(string namesection, string msg) => ErrorCallbackHandler?.Invoke(namesection, msg);
        public static event Action<string, string> ErrorCallbackHandler;
    }
}