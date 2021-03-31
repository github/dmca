using System.Collections;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace MelonLoader.Support
{
    public class Module : ISupportModule
    {
        public int GetActiveSceneIndex() => SceneManager.GetActiveScene().buildIndex;
        public object StartCoroutine(IEnumerator coroutine)
        {
            if (Main.comp != null) return Main.comp.StartCoroutine(coroutine);

            MelonLoaderComponent.QueuedCoroutines.Add(coroutine);
            return coroutine;
        }

        public void StopCoroutine(object coroutineToken)
        {
            if (Main.comp == null)
                MelonLoaderComponent.QueuedCoroutines.Remove(coroutineToken as IEnumerator);
            else
                Main.comp.StopCoroutine(coroutineToken as Coroutine);
        }

        public void UnityDebugLog(string msg) => Debug.Log(msg);
        public void Destroy() => MelonLoaderComponent.Destroy();
    }
}