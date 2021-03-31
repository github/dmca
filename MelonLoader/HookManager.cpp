#include <string>
#include "HookManager.h"
#include "MelonLoader_Base.h"
#include "MelonLoader.h"
#include "Console.h"
#include "Detours/detours.h"
#include "AssertionManager.h"
#include "Logger.h"
#include "Exports.h"
#include "DisableAnalytics.h"
#include <list>

#pragma region Core
std::vector<HookManager_Hook*>HookManager::HookTbl;
HookManager_Hook* HookManager::FindHook(void** target, void* detour)
{
	HookManager_Hook* returnval = NULL;
	size_t HookTblSize = HookTbl.size();
	if (HookTblSize > 0)
	{
		for (size_t i = 0; i < HookTblSize; i++)
		{
			HookManager_Hook* hook = HookTbl[i];
			if ((hook != NULL) && (hook->Target == target) && (hook->Detour == detour))
			{
				returnval = hook;
				break;
			}
		}
	}
	return returnval;
}

void HookManager::Hook(void** target, void* detour)
{
	if ((target != NULL) && (detour != NULL))
	{
		HookManager_Hook* hook = FindHook(target, detour);
		if (hook == NULL)
		{
			hook = new HookManager_Hook(target, detour);
			HookTbl.push_back(hook);
			INTERNAL_Hook(target, detour);
		}
	}
}

void HookManager::Unhook(void** target, void* detour)
{
	if ((target != NULL) && (detour != NULL))
	{
		HookManager_Hook* hook = FindHook(target, detour);
		if (hook != NULL)
		{
			HookTbl.erase(std::find(HookManager::HookTbl.begin(), HookManager::HookTbl.end(), hook));
			delete hook;
			INTERNAL_Unhook(target, detour);
		}
	}
}

void HookManager::UnhookAll()
{
	size_t HookTblSize = HookTbl.size();
	if (HookTblSize < 0)
		return;
	for (size_t i = 0; i < HookTblSize; i++)
	{
		HookManager_Hook* hook = HookTbl[i];
		if (hook != NULL)
		{
			INTERNAL_Unhook(hook->Target, hook->Detour);
			delete hook;
		}
	}
	HookTbl.clear();
	LoadLibraryW_Unhook();
}

void HookManager::INTERNAL_Hook(void** target, void* detour)
{
	if ((target != NULL) && (detour != NULL))
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(target, detour);
		DetourTransactionCommit();
	}
}

void HookManager::INTERNAL_Unhook(void** target, void* detour)
{
	if ((target != NULL) && (detour != NULL))
	{
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(target, detour);
		DetourTransactionCommit();
	}
}
#pragma endregion

#pragma region LoadLibraryW
LoadLibraryW_t HookManager::Original_LoadLibraryW = NULL;
void HookManager::LoadLibraryW_Hook()
{
	if (Original_LoadLibraryW == NULL)
	{
		Original_LoadLibraryW = LoadLibraryW;
		Hook(&(LPVOID&)Original_LoadLibraryW, Hooked_LoadLibraryW);
	}
}
void HookManager::LoadLibraryW_Unhook()
{
	if (Original_LoadLibraryW != NULL)
	{
		Unhook(&(LPVOID&)Original_LoadLibraryW, Hooked_LoadLibraryW);
		Original_LoadLibraryW = NULL;
	}
}
HMODULE __stdcall HookManager::Hooked_LoadLibraryW(LPCWSTR lpLibFileName)
{
	HMODULE lib = Original_LoadLibraryW(lpLibFileName);
	if (MelonLoader::IsGameIl2Cpp)
	{
		if (wcsstr(lpLibFileName, L"GameAssembly.dll"))
		{
			if (Il2Cpp::Setup(lib))
			{
				Hook(&(LPVOID&)Il2Cpp::il2cpp_init, Hooked_il2cpp_init);
				Hook(&(LPVOID&)Il2Cpp::il2cpp_unity_install_unitytls_interface, Hooked_il2cpp_unity_install_unitytls_interface);
			}
			LoadLibraryW_Unhook();
		}
	}
	else
	{
		Mono::IsOldMono = wcsstr(lpLibFileName, L"mono.dll");
		if (Mono::IsOldMono || wcsstr(lpLibFileName, L"mono-2.0-bdwgc.dll") || wcsstr(lpLibFileName, L"mono-2.0-sgen.dll") || wcsstr(lpLibFileName, L"mono-2.0-boehm.dll"))
		{
			Mono::Module = lib;
			if (Mono::Setup())
				Hook(&(LPVOID&)Mono::mono_jit_init_version, Hooked_mono_jit_init_version);
			LoadLibraryW_Unhook();
		}
	}
	return lib;
}
#pragma endregion

#pragma region il2cpp_unity_install_unitytls_interface
void HookManager::Hooked_il2cpp_unity_install_unitytls_interface(unitytls_interface* unitytlsInterfaceStruct)
{
	Il2Cpp::il2cpp_unity_install_unitytls_interface(unitytlsInterfaceStruct);
	UnityTLS::unitytlsinterface = unitytlsInterfaceStruct;
}
#pragma endregion

#pragma region il2cpp_init
Il2CppDomain* HookManager::Hooked_il2cpp_init(const char* name)
{
	if (Mono::Load() && Mono::Setup())
	{
		Hook(&(LPVOID&)Mono::mono_unity_get_unitytls_interface, UnityTLS::GetUnityTLSInterface);
		Mono::CreateDomain();
		Exports::AddInternalCalls();
		MelonLoader_Base::Initialize();
	}
	Il2Cpp::Domain = Il2Cpp::il2cpp_init(name);
	Unhook(&(LPVOID&)Il2Cpp::il2cpp_init, Hooked_il2cpp_init);
	return Il2Cpp::Domain;
}
#pragma endregion

#pragma region mono_jit_init_version
MonoDomain* HookManager::Hooked_mono_jit_init_version(const char* name, const char* version)
{
	Unhook(&(LPVOID&)Mono::mono_jit_init_version, Hooked_mono_jit_init_version);
	Mono::Domain = Mono::mono_jit_init_version(name, version);
	Mono::FixDomainBaseDir();
	Exports::AddInternalCalls();
	MelonLoader_Base::Initialize();
	return Mono::Domain;
}
#pragma endregion

#pragma region runtime_invoke
void* HookManager::Hooked_runtime_invoke(const void* method, void* obj, void** params, void** exc)
{
	const char* method_name = NULL;
	if (MelonLoader::IsGameIl2Cpp)
		method_name = Il2Cpp::il2cpp_method_get_name((Il2CppMethod*)method);
	else
		method_name = Mono::mono_method_get_name((MonoMethod*)method);
	if ((strstr(method_name, "Internal_ActiveSceneChanged") != NULL) || (!MelonLoader::IsGameIl2Cpp && (strstr(method_name, "UnityEngine.ISerializationCallbackReceiver.OnAfterDeserialize") != NULL)))
	{
		if (MelonLoader::IsGameIl2Cpp)
			Unhook(&(LPVOID&)Il2Cpp::il2cpp_runtime_invoke, Hooked_runtime_invoke);
		else
			Unhook(&(LPVOID&)Mono::mono_runtime_invoke, Hooked_runtime_invoke);
		MelonLoader_Base::Startup();
	}
	if (MelonLoader::IsGameIl2Cpp)
		return Il2Cpp::il2cpp_runtime_invoke((Il2CppMethod*)method, obj, params, (Il2CppObject**)exc);
	return Mono::mono_runtime_invoke((MonoMethod*)method, obj, params, (MonoObject**)exc);
}
#pragma endregion