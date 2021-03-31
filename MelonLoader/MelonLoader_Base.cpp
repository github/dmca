#include <Windows.h>
#include <string>
#include "MelonLoader_Base.h"
#include "MelonLoader.h"
#include "AssertionManager.h"
#include "Logger.h"
#include "HookManager.h"
#include "Il2Cpp.h"

bool MelonLoader_Base::HasInitialized = false;
MonoMethod* MelonLoader_Base::startup = NULL;

void MelonLoader_Base::Initialize()
{
	AssertionManager::Start("MelonLoader_Base.cpp", "MelonLoader_Base::Initialize");
	if (Mono::Domain != NULL)
	{
		std::string modhandlerpath = std::string(MelonLoader::GamePath) + "\\MelonLoader\\MelonLoader.ModHandler.dll";
		MonoAssembly* assembly = Mono::mono_domain_assembly_open(Mono::Domain, modhandlerpath.c_str());
		AssertionManager::Decide(assembly, "MelonLoader.ModHandler.dll");
		if (assembly != NULL)
		{
			MonoImage* image = Mono::mono_assembly_get_image(assembly);
			AssertionManager::Decide(assembly, "Image");
			if (image != NULL)
			{
				MonoClass* klass = Mono::mono_class_from_name(image, "MelonLoader", "MelonLoaderBase");
				AssertionManager::Decide(assembly, "MelonLoader.MelonLoaderBase");
				if (klass != NULL)
				{
					MonoMethod* initialize = Mono::mono_class_get_method_from_name(klass, "Initialize", NULL);
					AssertionManager::Decide(initialize, "Initialize");
					if (initialize != NULL)
					{
						MonoObject* exceptionObject = NULL;
						Mono::mono_runtime_invoke(initialize, NULL, NULL, &exceptionObject);
						if ((exceptionObject != NULL) && MelonLoader::DebugMode)
							Mono::LogExceptionMessage(exceptionObject);
						else
						{
							startup = Mono::mono_class_get_method_from_name(klass, "Startup", NULL);
							AssertionManager::Decide(startup, "Startup");
							if (MelonLoader::IsGameIl2Cpp)
								HookManager::Hook(&(LPVOID&)Il2Cpp::il2cpp_runtime_invoke, HookManager::Hooked_runtime_invoke);
							else
								HookManager::Hook(&(LPVOID&)Mono::mono_runtime_invoke, HookManager::Hooked_runtime_invoke);
							HasInitialized = true;
						}
					}
				}
			}
		}
	}
}

void MelonLoader_Base::Startup()
{
	if (startup != NULL)
	{
		MonoObject* exceptionObject = NULL;
		Mono::mono_runtime_invoke(startup, NULL, NULL, &exceptionObject);
		if ((exceptionObject != NULL) && MelonLoader::DebugMode)
			Mono::LogExceptionMessage(exceptionObject);
	}
}