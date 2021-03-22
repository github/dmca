#include <sys/cdefs.h>

__BEGIN_DECLS

CGImageRef LICreateDefaultIcon(NSInteger a, NSInteger b, NSInteger c);

__END_DECLS

#if __IPHONE_7_0
typedef NS_ENUM(NSUInteger, MIIconVariant) {
	                               // iphone  ipad
	MIIconVariantSmall,            // 29x29   29x29
	MIIconVariantSpotlight,        // 40x40   40x40
	MIIconVariantDefault,          // 62x62   78x78
	MIIconVariantGameCenter,       // 42x42   78x78
	MIIconVariantDocumentFull,     // 37x48   37x48
	MIIconVariantDocumentSmall,    // 37x48   37z48
	MIIconVariantSquareBig,        // 82x82   128x128
	MIIconVariantSquareDefault,    // 62x62   78x78
	MIIconVariantTiny,             // 20x20   20x20
	MIIconVariantDocument,         // 37x48   247x320
	MIIconVariantDocumentLarge,    // 37x48   247x320
	MIIconVariantUnknownGradient,  // 300x150 300x150
	MIIconVariantSquareGameCenter, // 42x42   42x42
	MIIconVariantUnknownDefault,   // 62x62   78x78

	/*
	 todo: find out what UnknownGradient and UnknownDefault are for.
	 UnknownGradient is a static gradient on iphone, and half of the
	 icon on ipad. UnknownDefault is the same thing as Default.
	*/
};
#else
typedef NS_ENUM(NSUInteger, MIIconVariant) {
	                               // iphone  ipad
	MIIconVariantSmall,            // 29x29   29x29
	MIIconVariantSpotlight,        // 29x29   50x50
	MIIconVariantDefault,          // 59x62   74x78
	MIIconVariantGameCenter,       // 44x45   74x78
	MIIconVariantDefaultGrayscale, // 59x62   74x78
	MIIconVariantDocumentFull,     // 31x37   64x64
	MIIconVariantDocumentSmall,    // 31x37   64x64
	MIIconVariantSquareBig,        // 104x104 145x148
	MIIconVariantSquareSmall,      // 71x71   92x94
	MIIconVariantTiny,             // 20x20   20x20
	MIIconVariantDocument,         // 31x37   320x320
	MIIconVariantDocumentLarge,    // 31x37   320x320
};
#endif
