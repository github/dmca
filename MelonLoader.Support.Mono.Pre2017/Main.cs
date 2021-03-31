using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace MelonLoader.Support
{
    internal static class Main
    {
        internal static bool IsDestroying = false;
        internal static GameObject obj = null;
        internal static MelonLoaderComponent comp = null;
        private static ISupportModule Initialize()
        {
            SceneManager.sceneLoaded += OnSceneLoad;
            return new Module();
        }
        private static void OnSceneLoad(Scene scene, LoadSceneMode mode)
        {
            if (obj == null) MelonLoaderComponent.Create();
            if (!scene.Equals(null)) SceneHandler.OnSceneLoad(scene.buildIndex);
        }
    }

    public class MelonLoaderComponent : MonoBehaviour
    {
        internal static readonly List<IEnumerator> QueuedCoroutines = new List<IEnumerator>();
        internal static void Create()
        {
            Main.obj = new GameObject();
            DontDestroyOnLoad(Main.obj);
            Main.comp = (MelonLoaderComponent)Main.obj.AddComponent(typeof(MelonLoaderComponent));
            Main.obj.transform.SetAsLastSibling();
            Main.comp.transform.SetAsLastSibling();
        }
        internal static void Destroy() { Main.IsDestroying = true; if (Main.obj != null) GameObject.Destroy(Main.obj); }
        void Awake()
        {
            foreach (var queuedCoroutine in QueuedCoroutines) StartCoroutine(queuedCoroutine);
            QueuedCoroutines.Clear();
        }
        void Start() => transform.SetAsLastSibling();
        void Update() { transform.SetAsLastSibling(); MelonHandler.OnUpdate(); }
        void FixedUpdate() => MelonHandler.OnFixedUpdate();
        void LateUpdate() => MelonHandler.OnLateUpdate();
        void OnGUI() => MelonHandler.OnGUI();
        void OnDestroy() { if (!Main.IsDestroying) Create(); }
        void OnApplicationQuit() { Destroy(); MelonHandler.OnApplicationQuit(); }
    }
}