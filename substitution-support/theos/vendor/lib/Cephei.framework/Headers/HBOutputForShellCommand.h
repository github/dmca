#include <sys/cdefs.h>

NS_ASSUME_NONNULL_BEGIN
__BEGIN_DECLS

/**
 * Executes a shell command and returns its output.
 *
 * @param command The shell command to run.
 * @param returnCode A pointer to an integer that will contain the return code of the command.
 * @returns The output of the provided command.
 */
NSString * _Nullable HBOutputForShellCommandWithReturnCode(NSString *command, int *returnCode);

/**
 * Executes a shell command and returns its output.
 *
 * @param command The shell command to run.
 * @returns The output of the provided command, or nil if the command returned with a code other
 * than 0.
 */
NSString * _Nullable HBOutputForShellCommand(NSString *command);

__END_DECLS
NS_ASSUME_NONNULL_END
