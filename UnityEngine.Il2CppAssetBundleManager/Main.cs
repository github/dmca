using MelonLoader;
using Il2CppSystem.IO;
using UnhollowerBaseLib;
using UnhollowerRuntimeLib;

namespace UnityEngine
{
    public class Il2CppAssetBundleManager
    {
        static Il2CppAssetBundleManager()
        {
            GetAllLoadedAssetBundles_NativeDelegateField = IL2CPP.ResolveICall<GetAllLoadedAssetBundles_NativeDelegate>("UnityEngine.AssetBundle::GetAllLoadedAssetBundles_Native");
            LoadFromFile_InternalDelegateField = IL2CPP.ResolveICall<LoadFromFile_InternalDelegate>("UnityEngine.AssetBundle::LoadFromFile_Internal(System.String,System.UInt32,System.UInt64)");
            LoadFromFileAsync_InternalDelegateField = IL2CPP.ResolveICall<LoadFromFileAsync_InternalDelegate>("UnityEngine.AssetBundle::LoadFromFileAsync_Internal");
            LoadFromMemory_InternalDelegateField = IL2CPP.ResolveICall<LoadFromMemory_InternalDelegate>("UnityEngine.AssetBundle::LoadFromMemory_Internal");
            LoadFromMemoryAsync_InternalDelegateField = IL2CPP.ResolveICall<LoadFromMemoryAsync_InternalDelegate>("UnityEngine.AssetBundle::LoadFromMemoryAsync_Internal");
            LoadFromStreamInternalDelegateField = IL2CPP.ResolveICall<LoadFromStreamInternalDelegate>("UnityEngine.AssetBundle::LoadFromStreamInternal");
            LoadFromStreamAsyncInternalDelegateField = IL2CPP.ResolveICall<LoadFromStreamAsyncInternalDelegate>("UnityEngine.AssetBundle::LoadFromStreamAsyncInternal");
            UnloadAllAssetBundlesDelegateField = IL2CPP.ResolveICall<UnloadAllAssetBundlesDelegate>("UnityEngine.AssetBundle::UnloadAllAssetBundles");
        }

		public static Il2CppAssetBundle[] GetAllLoadedAssetBundles()
		{
			if (GetAllLoadedAssetBundles_NativeDelegateField == null)
				throw new System.NullReferenceException("The GetAllLoadedAssetBundles_NativeDelegateField cannot be null.");
			System.IntPtr intPtr = GetAllLoadedAssetBundles_NativeDelegateField();
			Il2CppReferenceArray<Object> refarr = ((intPtr != System.IntPtr.Zero) ? new Il2CppReferenceArray<Object>(intPtr) : null);
			if (refarr == null)
				throw new System.NullReferenceException("The refarr cannot be null.");
			System.Collections.Generic.List<Il2CppAssetBundle> bundlelist = new System.Collections.Generic.List<Il2CppAssetBundle>();
			for (int i = 0; i < refarr.Length; i++)
				bundlelist.Add(new Il2CppAssetBundle(IL2CPP.Il2CppObjectBaseToPtrNotNull(refarr[i])));
			return bundlelist.ToArray();
		}

		public static Il2CppAssetBundle LoadFromFile(string path) => LoadFromFile(path, 0u, 0UL);
		public static Il2CppAssetBundle LoadFromFile(string path, uint crc) => LoadFromFile(path, crc, 0UL);
        public static Il2CppAssetBundle LoadFromFile(string path, uint crc, ulong offset)
        {
			if (string.IsNullOrEmpty(path))
				throw new System.ArgumentException("The input asset bundle path cannot be null or empty.");
			if (LoadFromFile_InternalDelegateField == null)
                throw new System.NullReferenceException("The LoadFromFile_InternalDelegateField cannot be null.");
            System.IntPtr intPtr = LoadFromFile_InternalDelegateField(IL2CPP.ManagedStringToIl2Cpp(path), crc, offset);
            return ((intPtr != System.IntPtr.Zero) ? new Il2CppAssetBundle(intPtr) : null);
        }

		public static AssetBundleCreateRequest LoadFromFileAsync(string path) => LoadFromFileAsync(path, 0u, 0UL);
		public static AssetBundleCreateRequest LoadFromFileAsync(string path, uint crc) => LoadFromFileAsync(path, crc, 0UL);
        public static AssetBundleCreateRequest LoadFromFileAsync(string path, uint crc, ulong offset)
        {
			if (string.IsNullOrEmpty(path))
				throw new System.ArgumentException("The input asset bundle path cannot be null or empty.");
			if (LoadFromFileAsync_InternalDelegateField == null)
                throw new System.NullReferenceException("The LoadFromFileAsync_InternalDelegateField cannot be null.");
            System.IntPtr intPtr = LoadFromFileAsync_InternalDelegateField(IL2CPP.ManagedStringToIl2Cpp(path), crc, offset);
            return ((intPtr != System.IntPtr.Zero) ? new AssetBundleCreateRequest(intPtr) : null);
        }

		public static Il2CppAssetBundle LoadFromMemory(Il2CppStructArray<byte> binary) => LoadFromMemory(binary, 0u);
        public static Il2CppAssetBundle LoadFromMemory(Il2CppStructArray<byte> binary, uint crc)
        {
			if (binary == null)
				throw new System.ArgumentException("The binary cannot be null or empty.");
			if (LoadFromMemory_InternalDelegateField == null)
                throw new System.NullReferenceException("The LoadFromMemory_InternalDelegateField cannot be null.");
            System.IntPtr intPtr = LoadFromMemory_InternalDelegateField(IL2CPP.Il2CppObjectBaseToPtrNotNull(binary), crc);
            return ((intPtr != System.IntPtr.Zero) ? new Il2CppAssetBundle(intPtr) : null);
        }

		public static AssetBundleCreateRequest LoadFromMemoryAsync(Il2CppStructArray<byte> binary) => LoadFromMemoryAsync(binary, 0u);
        public static AssetBundleCreateRequest LoadFromMemoryAsync(Il2CppStructArray<byte> binary, uint crc)
        {
			if (binary == null)
				throw new System.ArgumentException("The binary cannot be null or empty.");
			if (LoadFromMemoryAsync_InternalDelegateField == null)
                throw new System.NullReferenceException("The LoadFromMemoryAsync_InternalDelegateField cannot be null.");
            System.IntPtr intPtr = LoadFromMemoryAsync_InternalDelegateField(IL2CPP.Il2CppObjectBaseToPtrNotNull(binary), crc);
            return ((intPtr != System.IntPtr.Zero) ? new AssetBundleCreateRequest(intPtr) : null);
        }

		public static Il2CppAssetBundle LoadFromStream(Stream stream) => LoadFromStream(stream, 0u, 0u);
		public static Il2CppAssetBundle LoadFromStream(Stream stream, uint crc) => LoadFromStream(stream, crc, 0u);
        public static Il2CppAssetBundle LoadFromStream(Stream stream, uint crc, uint managedReadBufferSize)
        {
			if (stream == null)
				throw new System.ArgumentException("The stream cannot be null or empty.");
			if (LoadFromStreamInternalDelegateField == null)
                throw new System.NullReferenceException("The LoadFromStreamInternalDelegateField cannot be null.");
            System.IntPtr intPtr = LoadFromStreamInternalDelegateField(IL2CPP.Il2CppObjectBaseToPtrNotNull(stream), crc, managedReadBufferSize);
            return ((intPtr != System.IntPtr.Zero) ? new Il2CppAssetBundle(intPtr) : null);
        }

		public static AssetBundleCreateRequest LoadFromStreamAsync(Stream stream) => LoadFromStreamAsync(stream, 0u, 0u);
		public static AssetBundleCreateRequest LoadFromStreamAsync(Stream stream, uint crc) => LoadFromStreamAsync(stream, crc, 0u);
		public static AssetBundleCreateRequest LoadFromStreamAsync(Stream stream, uint crc, uint managedReadBufferSize)
		{
			if (stream == null)
				throw new System.ArgumentException("The stream cannot be null or empty.");
			if (LoadFromStreamAsyncInternalDelegateField == null)
				throw new System.NullReferenceException("The LoadFromStreamAsyncInternalDelegateField cannot be null.");
			System.IntPtr intPtr = LoadFromStreamAsyncInternalDelegateField(IL2CPP.Il2CppObjectBaseToPtrNotNull(stream), crc, managedReadBufferSize);
			return ((intPtr != System.IntPtr.Zero) ? new AssetBundleCreateRequest(intPtr) : null);
		}

		public static void UnloadAllAssetBundles(bool unloadAllObjects)
		{
			if (UnloadAllAssetBundlesDelegateField == null)
				throw new System.NullReferenceException("The UnloadAllAssetBundlesDelegateField cannot be null.");
			UnloadAllAssetBundlesDelegateField(unloadAllObjects);
		}

		private delegate System.IntPtr GetAllLoadedAssetBundles_NativeDelegate();
		private static readonly GetAllLoadedAssetBundles_NativeDelegate GetAllLoadedAssetBundles_NativeDelegateField;
		private delegate System.IntPtr LoadFromFile_InternalDelegate(System.IntPtr path, uint crc, ulong offset);
		private static readonly LoadFromFile_InternalDelegate LoadFromFile_InternalDelegateField;
		private delegate System.IntPtr LoadFromFileAsync_InternalDelegate(System.IntPtr path, uint crc, ulong offset);
		private static readonly LoadFromFileAsync_InternalDelegate LoadFromFileAsync_InternalDelegateField;
		private delegate System.IntPtr LoadFromMemory_InternalDelegate(System.IntPtr binary, uint crc);
		private static readonly LoadFromMemory_InternalDelegate LoadFromMemory_InternalDelegateField;
		private delegate System.IntPtr LoadFromMemoryAsync_InternalDelegate(System.IntPtr binary, uint crc);
		private static readonly LoadFromMemoryAsync_InternalDelegate LoadFromMemoryAsync_InternalDelegateField;
		private delegate System.IntPtr LoadFromStreamInternalDelegate(System.IntPtr stream, uint crc, uint managedReadBufferSize);
		private static readonly LoadFromStreamInternalDelegate LoadFromStreamInternalDelegateField;
		private delegate System.IntPtr LoadFromStreamAsyncInternalDelegate(System.IntPtr stream, uint crc, uint managedReadBufferSize);
		private static readonly LoadFromStreamAsyncInternalDelegate LoadFromStreamAsyncInternalDelegateField;
		private delegate System.IntPtr UnloadAllAssetBundlesDelegate(bool unloadAllObjects);
		private static readonly UnloadAllAssetBundlesDelegate UnloadAllAssetBundlesDelegateField;
	}

	public class Il2CppAssetBundle
    {
        private System.IntPtr bundleptr = System.IntPtr.Zero;
        public Il2CppAssetBundle(System.IntPtr ptr) { bundleptr = ptr; }
        static Il2CppAssetBundle()
        {
            get_isStreamedSceneAssetBundleDelegateField = IL2CPP.ResolveICall<get_isStreamedSceneAssetBundleDelegate>("UnityEngine.AssetBundle::get_isStreamedSceneAssetBundle");
            returnMainAssetDelegateField = IL2CPP.ResolveICall<returnMainAssetDelegate>("UnityEngine.AssetBundle::returnMainAsset");
            ContainsDelegateField = IL2CPP.ResolveICall<ContainsDelegate>("UnityEngine.AssetBundle::Contains");
			GetAllAssetNamesDelegateField = IL2CPP.ResolveICall<GetAllAssetNamesDelegate>("UnityEngine.AssetBundle::GetAllAssetNames");
			GetAllScenePathsDelegateField = IL2CPP.ResolveICall<GetAllScenePathsDelegate>("UnityEngine.AssetBundle::GetAllScenePaths");
			LoadAsset_InternalDelegateField = IL2CPP.ResolveICall<LoadAsset_InternalDelegate>("UnityEngine.AssetBundle::LoadAsset_Internal(System.String,System.Type)");
			LoadAssetAsync_InternalDelegateField = IL2CPP.ResolveICall<LoadAssetAsync_InternalDelegate>("UnityEngine.AssetBundle::LoadAssetAsync_Internal");
            LoadAssetWithSubAssets_InternalDelegateField = IL2CPP.ResolveICall<LoadAssetWithSubAssets_InternalDelegate>("UnityEngine.AssetBundle::LoadAssetWithSubAssets_Internal");
            LoadAssetWithSubAssetsAsync_InternalDelegateField = IL2CPP.ResolveICall<LoadAssetWithSubAssetsAsync_InternalDelegate>("UnityEngine.AssetBundle::LoadAssetWithSubAssetsAsync_Internal");
            UnloadDelegateField = IL2CPP.ResolveICall<UnloadDelegate>("UnityEngine.AssetBundle::Unload");
        }

		public bool isStreamedSceneAssetBundle
		{
			get
			{
                if (bundleptr == System.IntPtr.Zero)
                    throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
				if (get_isStreamedSceneAssetBundleDelegateField == null)
					throw new System.NullReferenceException("The get_isStreamedSceneAssetBundleDelegateField cannot be null.");
				return get_isStreamedSceneAssetBundleDelegateField(bundleptr);
			}
		}
		
		public Object mainAsset
        {
            get
            {
                if (bundleptr == System.IntPtr.Zero)
                    throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
                if (returnMainAssetDelegateField == null)
                    throw new System.NullReferenceException("The returnMainAssetDelegateField cannot be null.");
                System.IntPtr intPtr = returnMainAssetDelegateField(bundleptr);
                return ((intPtr != System.IntPtr.Zero) ? new Object(intPtr) : null);
            }
        }

        public bool Contains(string name)
		{
			if (bundleptr == System.IntPtr.Zero)
                throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
			if (string.IsNullOrEmpty(name))
				throw new System.ArgumentException("The input asset name cannot be null or empty.");
			if (ContainsDelegateField == null)
                throw new System.NullReferenceException("The ContainsDelegateField cannot be null.");
            return ContainsDelegateField(bundleptr, IL2CPP.ManagedStringToIl2Cpp(name));
        }

		public Il2CppStringArray AllAssetNames() => GetAllAssetNames();
		public Il2CppStringArray GetAllAssetNames()
		{
			if (bundleptr == System.IntPtr.Zero)
				throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
			if (GetAllAssetNamesDelegateField == null)
				throw new System.NullReferenceException("The GetAllAssetNamesDelegateField cannot be null.");
			System.IntPtr intPtr = GetAllAssetNamesDelegateField(bundleptr);
			return ((intPtr != System.IntPtr.Zero) ? new Il2CppStringArray(intPtr) : null);
		}

		public Il2CppStringArray AllScenePaths() => GetAllScenePaths();
        public Il2CppStringArray GetAllScenePaths()
        {
            if (bundleptr == System.IntPtr.Zero)
                throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
            if (GetAllScenePathsDelegateField == null)
                throw new System.NullReferenceException("The GetAllScenePathsDelegateField cannot be null.");
            System.IntPtr intPtr = GetAllScenePathsDelegateField(bundleptr);
            return ((intPtr != System.IntPtr.Zero) ? new Il2CppStringArray(intPtr) : null);
        }

		public Object Load(string name) => LoadAsset(name);
		public Object LoadAsset(string name) => LoadAsset<Object>(name);
		public T Load<T>(string name) where T : Object => LoadAsset<T>(name);
		public T LoadAsset<T>(string name) where T : Object
		{
			if (!UnhollowerSupport.IsGeneratedAssemblyType(typeof(T)))
				throw new System.NullReferenceException("The type must be a Generated Assembly Type.");
			System.IntPtr intptr = LoadAsset(name, Il2CppType.Of<T>().Pointer);
			return ((intptr != System.IntPtr.Zero) ? UnhollowerSupport.Il2CppObjectPtrToIl2CppObject<T>(intptr) : null);
		}
		public Object Load(string name, Il2CppSystem.Type type) => LoadAsset(name, type);
		public Object LoadAsset(string name, Il2CppSystem.Type type)
		{
            if (type == null)
                throw new System.NullReferenceException("The input type cannot be null.");
			System.IntPtr intptr = LoadAsset(name, type.Pointer);
			return ((intptr != System.IntPtr.Zero) ? new Object(intptr) : null);
		}
		public System.IntPtr Load(string name, System.IntPtr typeptr) => LoadAsset(name, typeptr);
		public System.IntPtr LoadAsset(string name, System.IntPtr typeptr)
		{
			if (bundleptr == System.IntPtr.Zero)
				throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
			if (string.IsNullOrEmpty(name))
				throw new System.ArgumentException("The input asset name cannot be null or empty.");
			if (typeptr == System.IntPtr.Zero)
				throw new System.NullReferenceException("The input type cannot be IntPtr.Zero");
			if (LoadAsset_InternalDelegateField == null)
				throw new System.NullReferenceException("The LoadAsset_InternalDelegateField cannot be null.");
			return LoadAsset_InternalDelegateField(bundleptr, IL2CPP.ManagedStringToIl2Cpp(name), typeptr);
		}

		public AssetBundleCreateRequest LoadAssetAsync(string name) => LoadAssetAsync<Object>(name);
		public AssetBundleCreateRequest LoadAssetAsync<T>(string name) where T : Object
		{
			if (!UnhollowerSupport.IsGeneratedAssemblyType(typeof(T)))
				throw new System.NullReferenceException("The type must be a Generated Assembly Type.");
			System.IntPtr intptr = LoadAssetAsync(name, Il2CppType.Of<T>().Pointer);
			return ((intptr != System.IntPtr.Zero) ? new AssetBundleCreateRequest(intptr) : null);
		}
		public AssetBundleCreateRequest LoadAssetAsync(string name, Il2CppSystem.Type type)
		{
			if (type == null)
				throw new System.NullReferenceException("The input type cannot be null.");
			System.IntPtr intptr = LoadAssetAsync(name, type.Pointer);
			return ((intptr != System.IntPtr.Zero) ? new AssetBundleCreateRequest(intptr) : null);
		}
		public System.IntPtr LoadAssetAsync(string name, System.IntPtr typeptr)
		{
			if (bundleptr == System.IntPtr.Zero)
				throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
			if (string.IsNullOrEmpty(name))
				throw new System.ArgumentException("The input asset name cannot be null or empty.");
			if (typeptr == System.IntPtr.Zero)
				throw new System.NullReferenceException("The input type cannot be IntPtr.Zero");
			if (LoadAssetAsync_InternalDelegateField == null)
				throw new System.NullReferenceException("The LoadAssetAsync_InternalDelegateField cannot be null.");
			return LoadAssetAsync_InternalDelegateField(bundleptr, IL2CPP.ManagedStringToIl2Cpp(name), typeptr);
		}

		public Il2CppReferenceArray<Object> LoadAll() => LoadAllAssets();
		public Il2CppReferenceArray<Object> LoadAllAssets() => LoadAllAssets<Object>();
		public Il2CppReferenceArray<T> LoadAll<T>() where T : Object => LoadAllAssets<T>();
		public Il2CppReferenceArray<T> LoadAllAssets<T>() where T : Object
		{
			if (!UnhollowerSupport.IsGeneratedAssemblyType(typeof(T)))
				throw new System.NullReferenceException("The type must be a Generated Assembly Type.");
			System.IntPtr intptr = LoadAllAssets(Il2CppType.Of<T>().Pointer);
			return ((intptr != System.IntPtr.Zero) ? new Il2CppReferenceArray<T>(intptr) : null);
		}
		public Il2CppReferenceArray<Object> LoadAll(Il2CppSystem.Type type) => LoadAllAssets(type);
		public Il2CppReferenceArray<Object> LoadAllAssets(Il2CppSystem.Type type)
		{
			if (type == null)
				throw new System.NullReferenceException("The input type cannot be null.");
			System.IntPtr intptr = LoadAllAssets(type.Pointer);
			return ((intptr != System.IntPtr.Zero) ? new Il2CppReferenceArray<Object>(intptr) : null);
		}
		public System.IntPtr LoadAll(System.IntPtr typeptr) => LoadAllAssets(typeptr);
        public System.IntPtr LoadAllAssets(System.IntPtr typeptr) => LoadAssetWithSubAssets(string.Empty, typeptr);
		public Il2CppReferenceArray<Object> LoadAssetWithSubAssets(string name) => LoadAssetWithSubAssets<Object>(name);
		public Il2CppReferenceArray<T> LoadAssetWithSubAssets<T>(string name) where T : Object
		{
			if (!UnhollowerSupport.IsGeneratedAssemblyType(typeof(T)))
				throw new System.NullReferenceException("The type must be a Generated Assembly Type.");
			System.IntPtr intptr = LoadAssetWithSubAssets(name, Il2CppType.Of<T>().Pointer);
			return ((intptr != System.IntPtr.Zero) ? new Il2CppReferenceArray<T>(intptr) : null);
		}
		public Il2CppReferenceArray<Object> LoadAssetWithSubAssets(string name, Il2CppSystem.Type type)
		{
			if (type == null)
				throw new System.NullReferenceException("The input type cannot be null.");
			System.IntPtr intptr = LoadAssetWithSubAssets(name, type.Pointer);
			return ((intptr != System.IntPtr.Zero) ? new Il2CppReferenceArray<Object>(intptr) : null);
		}
        public System.IntPtr LoadAssetWithSubAssets(string name, System.IntPtr typeptr)
        {
            if (bundleptr == System.IntPtr.Zero)
                throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
            if (string.IsNullOrEmpty(name))
                throw new System.ArgumentException("The input asset name cannot be null or empty.");
            if (typeptr == System.IntPtr.Zero)
                throw new System.NullReferenceException("The input type cannot be IntPtr.Zero");
            if (LoadAssetWithSubAssets_InternalDelegateField == null)
                throw new System.NullReferenceException("The LoadAssetWithSubAssets_InternalDelegateField cannot be null.");
            return LoadAssetWithSubAssets_InternalDelegateField(bundleptr, IL2CPP.ManagedStringToIl2Cpp(name), typeptr);
        }

		public AssetBundleCreateRequest LoadAssetWithSubAssetsAsync(string name) => LoadAssetWithSubAssetsAsync<Object>(name);
		public AssetBundleCreateRequest LoadAssetWithSubAssetsAsync<T>(string name) where T : Object
		{
			if (!UnhollowerSupport.IsGeneratedAssemblyType(typeof(T)))
				throw new System.NullReferenceException("The type must be a Generated Assembly Type.");
			System.IntPtr intptr = LoadAssetWithSubAssetsAsync(name, Il2CppType.Of<T>().Pointer);
			return ((intptr != System.IntPtr.Zero) ? new AssetBundleCreateRequest(intptr) : null);
		}
		public AssetBundleCreateRequest LoadAssetWithSubAssetsAsync(string name, Il2CppSystem.Type type)
		{
			if (type == null)
				throw new System.NullReferenceException("The input type cannot be null.");
			System.IntPtr intptr = LoadAssetWithSubAssetsAsync(name, type.Pointer);
			return ((intptr != System.IntPtr.Zero) ? new AssetBundleCreateRequest(intptr) : null);
		}
		public System.IntPtr LoadAssetWithSubAssetsAsync(string name, System.IntPtr typeptr)
		{
			if (bundleptr == System.IntPtr.Zero)
				throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
			if (string.IsNullOrEmpty(name))
				throw new System.ArgumentException("The input asset name cannot be null or empty.");
			if (typeptr == System.IntPtr.Zero)
				throw new System.NullReferenceException("The input type cannot be IntPtr.Zero");
			if (LoadAssetWithSubAssetsAsync_InternalDelegateField == null)
				throw new System.NullReferenceException("The LoadAssetWithSubAssetsAsync_InternalDelegateField cannot be null.");
			return LoadAssetWithSubAssetsAsync_InternalDelegateField(bundleptr, IL2CPP.ManagedStringToIl2Cpp(name), typeptr);
		}

		public void Unload(bool unloadAllLoadedObjects)
		{
			if (bundleptr == System.IntPtr.Zero)
				throw new System.NullReferenceException("The bundleptr cannot be IntPtr.Zero");
			if (UnloadDelegateField == null)
				throw new System.NullReferenceException("The UnloadDelegateField cannot be null.");
			UnloadDelegateField(bundleptr, unloadAllLoadedObjects);
		}

		private delegate bool get_isStreamedSceneAssetBundleDelegate(System.IntPtr u0040this);
		private static readonly returnMainAssetDelegate returnMainAssetDelegateField;
		private delegate System.IntPtr returnMainAssetDelegate(System.IntPtr u0040this);
		private static readonly get_isStreamedSceneAssetBundleDelegate get_isStreamedSceneAssetBundleDelegateField;
		private delegate bool ContainsDelegate(System.IntPtr u0040this, System.IntPtr name);
		private static readonly ContainsDelegate ContainsDelegateField;
		private delegate System.IntPtr GetAllAssetNamesDelegate(System.IntPtr u0040this);
		private static readonly GetAllAssetNamesDelegate GetAllAssetNamesDelegateField;
		private delegate System.IntPtr GetAllScenePathsDelegate(System.IntPtr u0040this);
		private static readonly GetAllScenePathsDelegate GetAllScenePathsDelegateField;
		private delegate System.IntPtr LoadAsset_InternalDelegate(System.IntPtr u0040this, System.IntPtr name, System.IntPtr type);
		private static readonly LoadAsset_InternalDelegate LoadAsset_InternalDelegateField;
		private delegate System.IntPtr LoadAssetAsync_InternalDelegate(System.IntPtr u0040this, System.IntPtr name, System.IntPtr type);
		private static readonly LoadAssetAsync_InternalDelegate LoadAssetAsync_InternalDelegateField;
		private delegate System.IntPtr LoadAssetWithSubAssets_InternalDelegate(System.IntPtr u0040this, System.IntPtr name, System.IntPtr type);
		private static readonly LoadAssetWithSubAssets_InternalDelegate LoadAssetWithSubAssets_InternalDelegateField;
		private delegate System.IntPtr LoadAssetWithSubAssetsAsync_InternalDelegate(System.IntPtr u0040this, System.IntPtr name, System.IntPtr type);
		private static readonly LoadAssetWithSubAssetsAsync_InternalDelegate LoadAssetWithSubAssetsAsync_InternalDelegateField;
		private delegate void UnloadDelegate(System.IntPtr u0040this, bool unloadAllObjects);
		private static readonly UnloadDelegate UnloadDelegateField;
	}
}
