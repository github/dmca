using System;
#pragma warning disable 0108

namespace MelonLoader
{
    public abstract class MelonMod : MelonBase
    {
        [Obsolete()]
        public MelonModInfoAttribute InfoAttribute { get => LegacyModInfo; }
        [Obsolete()]
        public MelonModGameAttribute[] GameAttributes { get => LegacyModGames; }

        public virtual void OnLevelIsLoading() {}
        public virtual void OnLevelWasLoaded(int level) {}
        public virtual void OnLevelWasInitialized(int level) {}
        public virtual void OnFixedUpdate() {}
    }
}