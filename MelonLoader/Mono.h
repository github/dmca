#pragma once
#include <Windows.h>
#include "UnityTLS.h"

struct MonoDomain;
struct MonoAssembly;
struct MonoImage;
struct MonoClass;
struct MonoMethod;
struct MonoProperty;
struct MonoString;
struct MonoObject;
struct MonoThread;
enum MonoDebugFormat
{
	MONO_DEBUG_FORMAT_NONE,
	MONO_DEBUG_FORMAT_MONO,
	MONO_DEBUG_FORMAT_DEBUGGER
};

typedef MonoDomain* (*mono_init_t) (const char* name);
typedef MonoDomain* (*mono_jit_init_t) (const char* name);
typedef MonoDomain* (*mono_jit_init_version_t) (const char* name, const char* version);
typedef void (*mono_jit_cleanup_t) (MonoDomain* domain);
typedef void (*mono_assembly_setrootdir_t) (const char* path);
typedef void (*mono_set_assemblies_path_t) (const char* path);
typedef void (*mono_set_config_dir_t) (const char* path);
typedef MonoAssembly* (*mono_domain_assembly_open_t) (MonoDomain* domain, const char* path);
typedef MonoImage* (*mono_assembly_get_image_t) (MonoAssembly* assembly);
typedef MonoClass* (*mono_class_from_name_t) (MonoImage* image, const char* name_space, const char* name);
typedef MonoMethod* (*mono_class_get_method_from_name_t) (MonoClass* klass, const char* name, int param_count);
typedef MonoObject* (*mono_runtime_invoke_t) (MonoMethod* method, void* obj, void** params, MonoObject** exec);
typedef const char* (*mono_method_get_name_t) (MonoMethod* method);
typedef void (*mono_add_internal_call_t) (const char* name, void* method);
typedef MonoThread* (*mono_thread_current_t)();
typedef void (*mono_thread_set_main_t)(MonoThread* thread);
typedef const char* (*mono_string_to_utf8_t) (MonoString* str);
typedef MonoString* (*mono_string_new_t) (MonoDomain* domain, const char* str);
typedef MonoProperty* (*mono_class_get_property_from_name_t) (MonoClass* klass, const char* name);
typedef MonoMethod* (*mono_property_get_get_method_t) (MonoProperty* prop);
typedef MonoClass* (*mono_object_get_class_t) (MonoObject* obj);
typedef int (*mono_runtime_set_main_args_t) (int argc, char* argv[]);
typedef void (*mono_domain_set_config_t)(MonoDomain* domain, const char* configpath, const char* filename);
typedef void (*mono_debug_init_t) (MonoDebugFormat format);
typedef void (*mono_debug_domain_create_t) (MonoDomain* domain);
typedef void (*mono_jit_parse_options_t)(int argc, char* argv[]);
typedef unitytls_interface* (*mono_unity_get_unitytls_interface_t)();

class Mono
{
public:
	static bool IsOldMono;
	static char* AssemblyPath;
	static char* BasePath;
	static char* ConfigPath;
	static HMODULE Module;
	static MonoDomain* Domain;
	static mono_init_t mono_init;
	static mono_jit_init_t mono_jit_init;
	static mono_jit_init_version_t mono_jit_init_version;
	static mono_jit_cleanup_t mono_jit_cleanup;
	static mono_assembly_setrootdir_t mono_assembly_setrootdir;
	static mono_set_assemblies_path_t mono_set_assemblies_path;
	static mono_set_config_dir_t mono_set_config_dir;
	static mono_domain_assembly_open_t mono_domain_assembly_open;
	static mono_assembly_get_image_t mono_assembly_get_image;
	static mono_class_from_name_t mono_class_from_name;
	static mono_class_get_method_from_name_t mono_class_get_method_from_name;
	static mono_runtime_invoke_t mono_runtime_invoke;
	static mono_method_get_name_t mono_method_get_name;
	static mono_add_internal_call_t mono_add_internal_call;
	static mono_thread_current_t mono_thread_current;
	static mono_thread_set_main_t mono_thread_set_main;
	static mono_string_to_utf8_t mono_string_to_utf8;
	static mono_string_new_t mono_string_new;
	static mono_class_get_property_from_name_t mono_class_get_property_from_name;
	static mono_property_get_get_method_t mono_property_get_get_method;
	static mono_object_get_class_t mono_object_get_class;
	static mono_runtime_set_main_args_t mono_runtime_set_main_args;
	static mono_domain_set_config_t mono_domain_set_config;
	static mono_unity_get_unitytls_interface_t mono_unity_get_unitytls_interface;

	static bool Load();
	static bool Setup();
	static void CreateDomain();
	static void FixDomainBaseDir();
	static const char* GetStringProperty(const char* propertyName, MonoClass* classType, MonoObject* classObject);
	static void LogExceptionMessage(MonoObject* exceptionObject, bool shouldThrow = false);
};
