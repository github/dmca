#pragma once
#include <Windows.h>
#include "UnityTLS.h"

struct Il2CppDomain;
struct Il2CppMethod
{
	void* targetMethod;
};
struct Il2CppObject;

typedef Il2CppDomain* (*il2cpp_init_t) (const char* name);
typedef Il2CppObject* (*il2cpp_runtime_invoke_t) (Il2CppMethod* method, void* obj, void** params, Il2CppObject** exc);
typedef const char* (*il2cpp_method_get_name_t) (Il2CppMethod* method);
typedef void (*il2cpp_unity_install_unitytls_interface_t)(unitytls_interface* unitytlsInterfaceStruct);

class Il2Cpp
{
public:
	static Il2CppDomain* Domain;
	static il2cpp_init_t il2cpp_init;
	static il2cpp_runtime_invoke_t il2cpp_runtime_invoke;
	static il2cpp_method_get_name_t il2cpp_method_get_name;
	static il2cpp_unity_install_unitytls_interface_t il2cpp_unity_install_unitytls_interface;

	static bool Setup(HMODULE mod);
};