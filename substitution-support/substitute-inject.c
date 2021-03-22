#include "substitute-inject.h"
#include "substitute-injector.h"

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <pid> <dylib>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    pid_t pid = strtoul(argv[1], NULL, 10);
    const char *library = argv[2];
    
    if (!SubHookProcess(pid, library)) {
        fprintf(stderr, "SubHookProcess() failed.\n");
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
