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

I am the [private] and copyright owner. [private] personally wrote both files. They are [private] own original modules in [private] HaleHound ™-CYD firmware and [private] have never assigned [private] rights in them to anyone.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

Two original source files [private] wrote for [private] HaleHound-CYD firmware:

1. jam_detect.cpp - [private] WiFi jamming-detection module (internal
name "WiFiGuardian"), first committed 2026-03-02.
2. eapol_capture.cpp - [private] on-device EAPOL / PMKID capture and
hashcat export module, first committed 2026-02-17.

Both are [private] own original modules. They are not part of the upstream ESP32-DIV project [private] firmware was built on; that project never had a jamming detector or an EAPOL/PMKID module, so I am the [private] of these two files. Both were published in HaleHound-CYD version 3.5.5 under the MIT License, Copyright © 2026 [private].

The copying is verifiable. The infringer's jam_detect.cpp still has [private] internal module name in a comment on line 2 (// Bruce Jam Detector (WiFiGuardian)); copies [private] threat code verbatim (case THREAT_JAMMING: return "JAMMED!"; at his line 59, [private] line 113), the same enum, the same channel set {1, 6, 11}, and [private] strings "Baseline learned" and "Threat cleared". His eapol_export.cpp copies [private] PMKID extractor, including the offsets eapolOff + 4 + 93 and eapolOff + 4 + 95 with [private] variable names kdLenOff and kdStart (his lines 61 and 63, [private] lines 284 and 290),
[private] guard keyDataLen < 22, and [private] output line WPA*01*. Neither file exists in the project he forked (pr3y/Bruce).

**If the original work referenced above is available online, please provide a URL.**

https://github.com/JesseCHale/HaleHound-CYD - [private] official project.

[private] v3.5.5 source was public under the MIT License and is mirrored online, so the infringing files can be compared directly against my originals, which carry [private] authorship header and [private] MIT copyright notice:

[private] jam_detect.cpp:
https://github.com/jaylikesbunda/HaleHound-CYD/blob/main/jam_detect.cpp
[private] eapol_capture.cpp:
https://github.com/jaylikesbunda/HaleHound-CYD/blob/main/eapol_capture.cpp
[private] MIT LICENSE (Copyright (c) 2026 [private]):
https://github.com/jaylikesbunda/HaleHound-CYD/blob/main/LICENSE

The infringer himself also keeps a copy of [private] original source, including [private] MIT LICENSE with [private] copyright notice intact, in his own repository - showing he had [private] licensed original and removed the notice when he copied the files into HeavyButter-Bruce-F0rK:

https://github.com/r13xr13/halehound-analysis/blob/main/src/jam_detect.cpp
https://github.com/r13xr13/halehound-analysis/blob/main/src/eapol_capture.cpp
https://github.com/r13xr13/halehound-analysis/blob/main/src/LICENSE

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

Only the following two files infringe (not the entire repository):

https://github.com/r13xr13/HeavyButter-Bruce-F0rK/blob/main/src/modules/wifi/jam_detect.cpp
https://github.com/r13xr13/HeavyButter-Bruce-F0rK/blob/main/src/modules/wifi/eapol_export.cpp

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

No

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

Yes

**Which license?**

mit-license

**How do you believe the license is being violated?**

The two files were released under the MIT License, Copyright © 2026 [private], in HaleHound-CYD v3.5.5. MIT permits reuse only if [private] copyright notice and the permission notice are kept in the copies. r13xr13 kept neither. He removed [private] authorship header ([private] "// HaleHound-CYD ... Module" header is replaced with "// Bruce Jam Detector"), credits [private] nowhere, and relicensed the whole project under AGPL-3.0, which I never applied and did not authorize. Because he did not meet the one condition of the MIT License, his use falls outside it and is unauthorized.

**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**

To comply with the MIT License, both jam_detect.cpp and eapol_export.cpp must include [private] copyright notice "Copyright © 2026 [private]" and the full MIT permission notice, restore [private] original authorship header, and credit [private] as the author instead of the false "Bruce Jam Detector" attribution. Alternatively, to fully resolve the infringement, remove both files from the repository.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

GitHub username: r13xr13 Repository: https://github.com/r13xr13/HeavyButter-Bruce-F0rK Email (from his own commits): [private]

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
