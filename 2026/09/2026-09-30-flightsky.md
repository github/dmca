Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

One or more repositories in this DMCA takedown notice has been processed in accordance with GitHub's prohibition on sharing unauthorized product licensing keys, software for generating unauthorized product licensing keys, and/or software for bypassing checks for product licensing keys.

You can learn more in [GitHub's Acceptable Use Policies](https://docs.github.com/en/github/site-policy/github-acceptable-use-policies).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

Ocean Float Mobile Limited is the developer and publisher of FlightSky – Flight Tracker and owns the relevant copyrights in the FlightSky application software. I am duly authorized to act on behalf of Ocean Float Mobile Limited and to submit this notice concerning the unauthorized circumvention of technological measures protecting the FlightSky software and its paid functionality.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The copyrighted work is FlightSky – Flight Tracker, a proprietary mobile flight-tracking application developed and published by Ocean Float Mobile Limited and made available for Android and iOS.

FlightSky provides real-time flight tracking and aviation information, including live aircraft monitoring, flight and airport information, and an immersive 3D flight-tracking experience.

The FlightSky software constitutes proprietary copyrighted software. The application operates on a freemium basis, with certain functionality made available only to users holding a valid premium subscription or other paid entitlement.

This complaint primarily concerns the unauthorized circumvention of technological measures incorporated into the Android version of the FlightSky software to control access to such paid functionality, rather than an allegation that the reported repository is a literal reproduction of FlightSky's complete source code.

**If the original work referenced above is available online, please provide a URL.**

Google Play:  
https://play.google.com/store/apps/details?id=com.live.flight.tracker

Apple App Store:  
https://apps.apple.com/us/app/flightsky-flight-tracker/id6753592996

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

https://github.com/rushiranpise/morphe-patches/blob/main/patches/src/main/kotlin/app/template/patches/flightsky/FlightskyPatch.kt

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

FlightSky implements technological measures within its Android application to control access to the copyrighted software and, in particular, to functionality reserved for authorized and paying users.

These measures include application licensing checks, subscription and premium-entitlement verification, and application-level premium access controls. FlightSky uses entitlement information to determine whether a user has a valid premium subscription or other authorized premium access. Where the required entitlement is not present, premium functionality remains unavailable to that user and the application continues to operate under the applicable free-user restrictions, including its advertising model.

The application also incorporates licensing checks intended to verify authorized use of the distributed application. Together, these mechanisms ensure that users cannot normally obtain access to FlightSky's paid functionality merely by changing a local preference or representing themselves as premium users; the application verifies the relevant licensing and entitlement status before providing such access.

These technological measures therefore effectively control access to paid functionality forming part of the proprietary and copyrighted FlightSky software.

**How is the accused project designed to circumvent your technological protection measures?**

The accused project contains a FlightSky-specific bytecode patch expressly designed to defeat the technological measures described above.

The reported file includes a patch named flightskyBypassPairipPatch, which is expressly described by its author as “Disables PairIP license checks.” The code modifies the relevant licensing components so that license-validation methods are bypassed or prevented from performing their intended checks.

The same file also contains a patch named flightskyUnlockPremiumPatch, expressly described as “Unlocks premium features in app.” This patch modifies FlightSky's premium-entitlement logic by replacing the relevant Adapty isActive check so that it returns a positive/active result regardless of the user's legitimate entitlement status. It also modifies premium-entitlement handling so that premium access values are treated as true.

In addition, the patch directly modifies FlightSky's application preferences by setting user_premium_premium to true and ads_enabled to false. The effect is to cause a modified version of FlightSky to treat a user as a premium user and disable advertising irrespective of whether that user has actually purchased or obtained the required premium entitlement.

These modifications are therefore specifically designed to bypass FlightSky's licensing and premium-access controls and to enable access to functionality reserved for authorized paying users without satisfying the technological conditions imposed by the original application.

[private]

The accused project contains a FlightSky-specific bytecode patch expressly designed to defeat the technological measures described above.

The reported file includes a patch named flightskyBypassPairipPatch, which is expressly described by its author as “Disables PairIP license checks.” The code modifies the relevant licensing components so that license-validation methods are bypassed or prevented from performing their intended checks.

The same file also contains a patch named flightskyUnlockPremiumPatch, expressly described as “Unlocks premium features in app.” This patch modifies FlightSky's premium-entitlement logic by replacing the relevant Adapty isActive check so that it returns a positive/active result regardless of the user's legitimate entitlement status. It also modifies premium-entitlement handling so that premium access values are treated as true.

In addition, the patch directly modifies FlightSky's application preferences by setting user_premium_premium to true and ads_enabled to false. The effect is to cause a modified version of FlightSky to treat a user as a premium user and disable advertising irrespective of whether that user has actually purchased or obtained the required premium entitlement.

These modifications are therefore specifically designed to bypass FlightSky's licensing and premium-access controls and to enable access to functionality reserved for authorized paying users without satisfying the technological conditions imposed by the original application.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

[private]

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
