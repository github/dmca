Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am one of the [private] and copyright owners of Hybrid Mount, and I am authorized to submit this notice on behalf of Hybrid Mount [private]. Hybrid Mount [private] publish the official project and release packages through https://github.com/Hybrid-Mount/meta-hybrid_mount.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The original copyrighted work is Hybrid Mount / Hybrid Mount Lite, a mount orchestration metamodule for KernelSU and APatch developed by Hybrid Mount Developers.

Hybrid Mount Lite includes a compiled Rust binary named `hybrid-mount`, installation and service scripts, module metadata, configuration files, WebUI assets, localized frontend files, project images, and related package resources. The official release package at issue is `Hybrid-Mount-Lite-4.2.0-1815.zip`.

Official project repository:  
https://github.com/Hybrid-Mount/meta-hybrid_mount

Official release page:  
https://github.com/Hybrid-Mount/meta-hybrid_mount/releases/tag/v4.2.0

Official copyrighted release asset:  
https://github.com/Hybrid-Mount/meta-hybrid_mount/releases/download/v4.2.0/Hybrid-Mount-Lite-4.2.0-1815.zip

The package contains, among other files, `binaries/hybrid-mount`, `customize.sh`, `metainstall.sh`, `metamount.sh`, `service.sh`, `module.prop`, `webroot/index.html`, WebUI assets under `webroot/assets/`, and donation/payment image assets under `webroot/assets/donate/`.

**If the original work referenced above is available online, please provide a URL.**

https://github.com/Hybrid-Mount/meta-hybrid_mount/releases/download/v4.2.0/Hybrid-Mount-Lite-4.2.0-1815.zip

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

https://github.com/fixz232/ApkeSU/blob/ApkeSU/userspace/ksud/builtin/hybrid-mount-lite.zip  
https://raw.githubusercontent.com/fixz232/ApkeSU/ApkeSU/userspace/ksud/builtin/hybrid-mount-lite.zip

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

No

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/SamrtinSinale/ApkeSB  
https://github.com/matsuzaka-yuki/ApkeSU

**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**

**Is the work licensed under an open source license?**

Yes

**Which license?**

apache-license-2.0

**How do you believe the license is being violated?**

The copied Hybrid Mount Lite v4.2.0 package was published under Apache License 2.0. Apache-2.0 allows redistribution only subject to compliance with its conditions, including preserving copyright and attribution notices, providing the license text, and identifying modifications where applicable.

The ApkeSU repository stores and embeds our official Hybrid Mount Lite release package as `userspace/ksud/builtin/hybrid-mount-lite.zip` and integrates it as a built-in ApkeSU feature. The repository does not clearly identify this package as an unmodified third-party Hybrid Mount release, does not provide clear Hybrid Mount license compliance at the point of redistribution, and presents the copied package as part of ApkeSU’s own built-in functionality.

Hybrid Mount is also moving future releases to GPLv3. GPLv3 has additional redistribution obligations for binaries/object code, including corresponding source and preservation of license notices. Given that ApkeSU already bundles multiple GPL-licensed and third-party binary components, its current distribution model appears incompatible with proper license compliance unless it clearly separates third-party works and satisfies each project’s license obligations.

Therefore, we believe the repository violates our license terms by redistributing our complete release package without proper authorization, attribution, notice preservation, and license-compliance handling.

**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**

To bring the project into compliance, the repository owner should remove the embedded copy of Hybrid Mount Lite from the ApkeSU repository and stop redistributing it as a built-in component.

Specifically, they should:

1. Delete `userspace/ksud/builtin/hybrid-mount-lite.zip` from the repository and its branches.  
2. Remove code that embeds or distributes the ZIP as a built-in ApkeSU component, including references such as `include_bytes!("../builtin/hybrid-mount-lite.zip")`.  
3. Remove UI/CLI functionality that presents Hybrid Mount Lite as an ApkeSU built-in component unless they obtain explicit permission from Hybrid Mount Developers.  
4. Remove any extracted copies of Hybrid Mount files, including the `hybrid-mount` binary, WebUI assets, scripts, module metadata, and donation/payment images.  
5. If they wish to support Hybrid Mount, they should link users to the official Hybrid Mount repository or official release page instead of bundling and redistributing our release package.  
6. If they believe they are permitted to redistribute it under the applicable license, they must at minimum provide complete and clear license compliance, including preserving copyright and attribution notices, providing the applicable license text, clearly identifying Hybrid Mount Lite as a third-party work, and satisfying any source-code and notice obligations applicable to the redistributed version.

Because the project embeds the complete official release package as a built-in component, our preferred remedy is removal of the copied package and replacement with a link to the official Hybrid Mount release.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

I do not have reliable direct contact information for the alleged infringer. The infringing repository is owned by the GitHub account `fixz232`:

https://github.com/fixz232  
https://github.com/fixz232/ApkeSU  
Public commit metadata in the repository includes the [private] `[private]` and the [private] `[private]`, but I cannot verify whether this is the repository owner’s [private] [private] [private].

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
