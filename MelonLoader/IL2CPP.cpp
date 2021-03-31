#include "Il2Cpp.h"
#include "AssertionManager.h"
#include "Mono.h"
#include "Logger.h"

Il2CppDomain* Il2Cpp::Domain = NULL;
il2cpp_init_t Il2Cpp::il2cpp_init = NULL;
il2cpp_runtime_invoke_t Il2Cpp::il2cpp_runtime_invoke = NULL;
il2cpp_method_get_name_t Il2Cpp::il2cpp_method_get_name = NULL;
il2cpp_unity_install_unitytls_interface_t Il2Cpp::il2cpp_unity_install_unitytls_interface = NULL;

bool Il2Cpp::Setup(HMODULE mod)
{
	AssertionManager::Start("Il2Cpp.cpp", "Il2Cpp::Setup");

	il2cpp_init = (il2cpp_init_t)AssertionManager::GetExport(mod, "il2cpp_init");
	il2cpp_runtime_invoke = (il2cpp_runtime_invoke_t)AssertionManager::GetExport(mod, "il2cpp_runtime_invoke");
	il2cpp_method_get_name = (il2cpp_method_get_name_t)AssertionManager::GetExport(mod, "il2cpp_method_get_name");
	il2cpp_unity_install_unitytls_interface = (il2cpp_unity_install_unitytls_interface_t)AssertionManager::GetExport(mod, "il2cpp_unity_install_unitytls_interface");

	return !AssertionManager::Result;
}