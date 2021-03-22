# Logos
Logos is a Perl regex-based preprocessor that simplifies the boilerplate code needed to create hooks for Objective-C methods and C functions with an elegant Objective-C-like syntax. It’s most commonly used along with the [Theos](https://theos.github.io/) build system, which was originally developed to create jailbreak tweaks. Logos was once integrated in the same Git repo as Theos, but now has been decoupled from Theos to its own repo.

Logos aims to provide an interface for [Cydia Substrate](https://cydiasubstrate.com/) by default, but can be configured to directly use the Objective-C runtime.

Documentation is available on the [Logos page](http://iphonedevwiki.net/index.php/Logos) of iPhone Dev Wiki. 

## Example
### Source file: Tweak.x
```logos
%hook NSObject

- (NSString *)description {
	return [%orig stringByAppendingString:@" (of doom)"];
}

%new - (void)helloWorld {
	NSLog(@"Awesome!");
}

%end
```

### Logos-processed output: Tweak.x.m
*(Modified for brevity)*

```objc
#include <substrate.h>

static NSString *_logos_method$_ungrouped$NSObject$description(NSObject *self, SEL _cmd) {
	return [_logos_orig$_ungrouped$NSObject$description(self, _cmd) stringByAppendingString:@" (of doom)"];
}

static void _logos_method$_ungrouped$NSObject$helloWorld(NSObject * self, SEL _cmd) {
	NSLog(@"Awesome!");
}

static __attribute__((constructor)) void _logosLocalInit() {
	Class _logos_class$_ungrouped$NSObject = objc_getClass("NSObject");

	MSHookMessageEx(_logos_class$_ungrouped$NSObject, @selector(description), (IMP)&_logos_method$_ungrouped$NSObject$description, (IMP*)&_logos_orig$_ungrouped$NSObject$description);

	class_addMethod(_logos_class$_ungrouped$NSObject, @selector(helloWorld), (IMP)&_logos_method$_ungrouped$NSObject$helloWorld, "v@:");
}
```

## License
Licensed under the GNU General Public License, version 3.0, with an exception that projects created using Logos are not required to use the same license. Refer to [LICENSE.md](LICENSE.md).
