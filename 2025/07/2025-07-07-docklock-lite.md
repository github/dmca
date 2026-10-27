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

[private] the [private] [private] and [private] copyright holder of the macOS application DockLock Lite. The app is distributed through the Apple Mac App Store under [private] [private]. [private] created the software and hold exclusive rights to its code, binary, and all related intellectual property. I am submitting this DMCA notice on [private] [private] behalf.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The infringed work is [private] macOS application titled DockLock Lite, which is a proprietary utility that manages Dock placement in multi-monitor setups. It includes original source code, binary logic, and application structure. The script located at the GitHub repository https://github.com/QiuChenly/InjectLib/blob/main/tool/docklock.sh is designed to tamper with, inject code into, and alter [private] application without authorization. This constitutes an unauthorized derivative modification of a commercial product and a violation of my exclusive rights under copyright law, as well as a circumvention of technical protection measures under Section 1201 of the DMCA.

**If the original work referenced above is available online, please provide a URL.**

https://apps.apple.com/us/app/docklock-lite/id6741814079?mt=12

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

https://github.com/QiuChenly/InjectLib/blob/main/tool/docklock.sh

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

[private] app "DockLock Lite" includes several technological protection measures that effectively control access to and modification of the software:
- Apple App Store DRM: The app is distributed exclusively through the Mac App Store, which wraps binaries in FairPlay DRM, restricting unauthorized distribution and modification.  
- Code Signing: The app is signed using [private] [private]. Any post-installation modification (such as binary patching or library injection) breaks the code signature and normally prevents the app from running, unless circumvented.  
- Gatekeeper and Quarantine Attributes: macOS verifies the integrity and origin of downloaded applications using Gatekeeper and xattr quarantine flags. The script in question explicitly removes these (xattr -cr) to suppress warnings or blocks.  
- System Integrity Protections (SIP): By injecting code into the app and re-signing it with a dummy identity (codesign -s -), the script bypasses Apple's runtime protections and app validation mechanisms.

These measures are designed to control access and preserve the integrity of the distributed app. The script being reported is an intentional circumvention of these protections, in violation of DMCA §1201.

**How is the accused project designed to circumvent your technological protection measures?**

The accused project provides a script (docklock.sh) that is explicitly designed to circumvent the technological protection measures applied to [private] macOS app DockLock Lite. The script performs the following circumventions:  
1. Bypasses Code Signing: It injects a malicious dynamic library (CoreInject.dylib) into [private] signed application binary and then forcibly re-signs the app using a null (-) identity via codesign, thereby bypassing Apple's code integrity verification.  
2. Strips Quarantine Metadata: It removes macOS quarantine attributes using xattr -cr, a known method to suppress Gatekeeper's integrity checks and warnings to users about untrusted modifications.  
3. Circumvents DRM and Distribution Controls: By altering the app binary post-installation and embedding unauthorized behavior, it defeats the App Store's DRM and undermines distribution restrictions imposed by Apple's FairPlay system.  
4. Neutralizes System Integrity Protection (SIP): Through sudo-level operations and dylib injection, it performs modifications that would not be possible under SIP without circumvention.

These steps are not incidental - they are explicitly and programmatically implemented to disable protections that are meant to prevent unauthorized modification, redistribution, or tampering with the app. This constitutes a direct violation of the anti-circumvention provisions under §1201 of the DMCA.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/powerjiayun/InjectLib_20240616/blob/main/tool/docklock.sh  
https://github.com/funcdfs/InjectLib/blob/main/tool/docklock.sh

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

No, I do not have the alleged infringer's contact information. I am reporting the infringement based on publicly available content on GitHub, and the user has not provided any identifiable contact details in their GitHub profile or repository.

Additionally, the same actors appear to be distributing a modified, unauthorized version of [private] application "DockLock Lite" via a public [private] channel. While this occurs outside GitHub, it reinforces the intent behind the repository - not just to experiment, but to redistribute tampered versions of [private] software for use without consent or control. If needed, I can provide logs or screenshots confirming this.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
