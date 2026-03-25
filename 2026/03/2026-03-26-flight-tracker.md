While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

Impala Studios is the sole author and copyright owner of The Flight Tracker for Android, a proprietary Android application developed entirely by its [private]. No assignment or transfer of rights has been made. This notice is submitted by the Impala Studios [private], acting on behalf of and with full authority from Impala Studios as the rights holder.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The Flight Tracker for Android is a proprietary Android mobile application developed and owned exclusively by Impala Studios. The application comprises original source code, software architecture, business logic, and associated configuration assets. The repository subject to this notice contains an unauthorised decompiled version of this application, reconstructing its source code through reverse engineering of the compiled Android binary. Critically, the decompiled content includes embedded API keys, authentication tokens, and other confidential secrets that form part of the application's proprietary configuration — the exposure of which poses a direct security risk to Impala Studios and its users, in addition to constituting copyright infringement.

**If the original work referenced above is available online, please provide a URL.**
 
https://play.google.com/store/apps/details?id=com.flistholding.flightplus&hl=en  
https://www.impalastudios.com/the-flight-tracker/

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/wizdom13/Track

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Impala Studios employs multiple technological measures to control access to the copyrighted work. The application's source code is maintained in a private version control repository, access to which is strictly restricted to authorised employees through role-based access controls and authentication mechanisms. The compiled application binary is distributed exclusively through the Google Play Store, which applies its own access controls, and is not made available through any other channel. The compiled binary is further protected by standard Android binary compilation, which transforms the original source code into machine-readable bytecode — a technical barrier against casual reproduction. The decompiled content in the repository subject to this notice was obtained only by circumventing this technical protection, without authorisation from Impala Studios.

**How is the accused project designed to circumvent your technological protection measures?**

The repository subject to this notice contains files with the .smali extension, which are the direct output of disassembling the compiled Android binary of Flight Tracker for Android. Smali is the human-readable representation of Dalvik bytecode (DEX format) — the compiled form in which Android applications are distributed. The Android compilation process transforms the original proprietary source code into this bytecode, constituting a technical protection measure that prevents direct access to the underlying source. To produce the .smali files present in this repository, the responsible party necessarily employed disassembly tooling — most commonly Apktool or equivalent — to deconstruct the application binary and reconstruct its logic in human-readable form. This process directly and deliberately circumvents the technical barrier imposed by binary compilation and was carried out without any authorisation from Impala Studios. The resulting repository makes the disassembled code, including embedded API keys and confidential credentials, publicly accessible, materially compounding the harm caused by the initial circumvention. Among the exposed credentials, Impala Studios has positively identified its own OAuth client ID and client secret, which are unambiguously attributable to Impala Studios through our registered developer accounts. The presence of these credentials in the repository constitutes both a direct security threat and irrefutable evidence that the disassembled material originates from Impala Studios' proprietary application.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
