Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

One or more repositories in this DMCA takedown notice has been processed in accordance with GitHub's prohibition on sharing unauthorized product licensing keys, software for generating unauthorized product licensing keys, and/or software for bypassing checks for product licensing keys.

You can learn more in [GitHub's Acceptable Use Policies](https://docs.github.com/en/github/site-policy/github-acceptable-use-policies).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am the [private] and authorized representative of Unicorn Soft, Inc., the owner and developer of Unicorn Pro, a proprietary commercial Android application. Unicorn Soft, Inc. owns the copyrights and exclusive rights in the Unicorn Pro software, including its application code, license verification logic, and paid-feature access control implementation.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

Unicorn Pro is a proprietary commercial software application developed and owned by Unicorn Soft, Inc. The Android package name is com.unicornsoft.android.unicornpro.

The application includes copyrighted software code and technical protection measures that control access to paid Pro features. These measures include license verification, paid-feature access control, and native/JNI-based license-related flows involving components such as License, LicenseManager, LicenseManager.Auth, LicenseManager.Device, LicenseTracker, native_GetLicense, native_StartAppProcess, and native_SignIn.

The reported GitHub release contains a patch specifically targeting Unicorn Pro version 1.30.471 and is designed to bypass or alter the app’s license verification behavior so that the app reports an active Pro license without authorization.

**If the original work referenced above is available online, please provide a URL.**

https://getunicorn.app/ko/purchase

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

https://github.com/AmpleReVanced/revanced-patches/releases/tag/v1.2.0-dev.7

https://github.com/AmpleReVanced/revanced-patches/blob/v1.2.0-dev.5/patches/src/main/kotlin/app/revanced/patches/unicorn/shared/Constants.kt

https://github.com/AmpleReVanced/revanced-patches/blob/v1.2.0-dev.5/patches/src/main/kotlin/app/revanced/patches/unicorn/pro/UnlockProFeaturesPatch.kt

https://github.com/AmpleReVanced/revanced-patches/blob/v1.2.0-dev.5/extensions/unicorn/src/main/java/app/revanced/extension/unicorn/core/LicenseNative.java

https://github.com/AmpleReVanced/revanced-patches/blob/v1.2.0-dev.5/extensions/unicorn/src/main/java/app/revanced/extension/unicorn/core/NativeRouter.java

https://github.com/AmpleReVanced/revanced-patches/blob/v1.2.0-dev.5/patches/src/main/kotlin/app/revanced/patches/unicorn/tracker/DisableSentryPatch.kt

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Unicorn Pro uses technological protection measures to control access to its paid Pro features and copyrighted software functionality.

These measures include license verification, paid-feature authorization checks, and native/JNI-based license validation flows within the Android application. The app verifies whether a user has a valid purchase, subscription, or license before enabling Pro functionality.

The relevant license-related components include License, LicenseManager, LicenseManager.Auth, LicenseManager.Device, LicenseTracker, and native/JNI methods such as native_GetLicense, native_StartAppProcess, and native_SignIn.

These measures effectively control access by preventing users without a valid license from accessing paid Pro features. In normal operation, the app relies on the original license verification flow and license state returned by these components before enabling Pro functionality.

**How is the accused project designed to circumvent your technological protection measures?**

The accused project includes a ReVanced patch named “Unlock Pro features” targeting Unicorn Pro.

The patch is designed to modify Unicorn Pro’s APK bytecode and reroute license-related native/JNI calls away from the original license verification flow to injected code included in the patch extension.

Specifically, the patch targets license-related methods in classes such as License, LicenseManager, LicenseManager.Auth, LicenseManager.Device, and LicenseTracker. Instead of allowing the original Unicorn Pro license verification logic to determine the license state, the patch redirects those calls through NativeBridge, NativeRouter, and LicenseNative.

The injected LicenseNative implementation appears to construct or return an active Pro license state, including values such as LICENSE_SCOPE = pro, LICENSE_EXPIRED_AT = 2099-12-31T23:59:59.000Z, expired = false, trial = false, life = Integer.MAX_VALUE, and maxDevice = Integer.MAX_VALUE.

As a result, the modified app can report an active Pro license and enable paid Pro functionality without a valid purchase, subscription, or authorization from Unicorn Soft, Inc. This bypasses Unicorn Pro’s technological measures for controlling access to its paid copyrighted software functionality.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/cmanixli/AmpleReVanced

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

GitHub repository owner/user:  
https://github.com/AmpleReVanced

Repository:
https://github.com/AmpleReVanced/revanced-patches

Identified commit contributor:  
[private]

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
