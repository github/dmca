#include <CoreFoundation/CoreFoundation.h>
#include <dlfcn.h>

void (*MSUPurgeSuspendedUpdate)(void *) = NULL;

int main(int argc, char **argv) {
	void *lib = dlopen("/System/Library/PrivateFrameworks/MobileSoftwareUpdate.framework/MobileSoftwareUpdate", RTLD_LAZY);
	MSUPurgeSuspendedUpdate = dlsym(lib, "MSUPurgeSuspendedUpdate");
	MSUPurgeSuspendedUpdate(NULL);
	return EXIT_SUCCESS;
}
