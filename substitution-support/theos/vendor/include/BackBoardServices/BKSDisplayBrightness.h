#include <sys/cdefs.h>

typedef struct BKSDisplayBrightnessTransaction *BKSDisplayBrightnessTransactionRef;

__BEGIN_DECLS

BKSDisplayBrightnessTransactionRef BKSDisplayBrightnessTransactionCreate(CFAllocatorRef allocator);

float BKSDisplayBrightnessGetCurrent();
void BKSDisplayBrightnessSet(float brightness, NSInteger unknown);

__END_DECLS
