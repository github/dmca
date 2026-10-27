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
  
I am [private], the [private] and principal copyright owner of the sunnypilot software. I am the [private] and [private] of SUNNYPILOT LLC. All sunnypilot-original source code is copyrighted under "Copyright (c) 2021-, [private], sunnypilot, and a number of other contributors" and licensed under a custom MIT License (see LICENSE.md in the sunnypilot repository). The sunnypilot project includes contributions from both internal and external contributors who have submitted their work via pull requests under the same license terms. As the [private] and [private], I am authorized to act on behalf of all contributors whose work is included in the repository. I am filing this notice as the copyright owner.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
sunnypilot is an advanced driver assistance system (ADAS) that extends openpilot (by comma.ai). The copyrighted works are all sunnypilot-original source files — code authored by [private] ([private]) and contributors under [private] direction, distinct from upstream openpilot code.  
  
The copyrighted works include:  
  
- Custom vehicle control extensions: controlsd_ext.py, longitudinal_planner.py, cruise_helpers.py, cruise_ext.py, car_specific.py, speed_limit_resolver.py, and related modules  
- Map daemon (mapd): mapd_manager.py, mapd_installer.py, update_version.py, live_map_data/ directory  
- Model inference pipeline: modeld_v2/ directory (parse_model_outputs.py, fill_model_msg.py, meta_20hz.py, meta_helper.py, camera_offset_helper.py, compile_modeld.py), model runners (model_runner.py, tinygrad_runner.py, helpers.py)  
- sunnylink connectivity service: sunnylink/ directory including backups/manager.py and API client  
- Neural network lateral control (nnlc): module and associated tests  
- Dynamic Experimental Controller (dec): module and associated tests  
- Live delay calibration: livedelay/ directory (lagd_toggle.py, helpers.py)  
- Custom UI layouts: selfdrive/ui/sunnypilot/ directory  
- Custom Cap'n Proto schemas: cereal/custom.capnp containing sunnypilot-defined structs with unique schema identifiers (e.g., CarParamsSP @0x80ae746ee2596b11, SelfdriveStateSP @0x81c2f05a394cf4af, BackupManagerSP @0xf98d843bfd7004a3, and 12 others)  
- Quiet mode UI feature, event handling extensions, torqued extensions, and all associated test files  
- The custom MIT License itself (LICENSE.md)  
  
Each sunnypilot-original file contains the header: "Copyright (c) 2021-, [private], sunnypilot, and a number of other contributors. This file is part of sunnypilot and is licensed under the MIT License."  
  
The infringing repository (https://github.com/IQLvbs/openpilot) is not a GitHub fork — the infringer created a fresh repository and pushed sunnypilot-derived code in single squashed commits to avoid GitHub's fork relationship. The infringement includes:  
  
COPYRIGHT HEADER STRIPPING: 39+ sunnypilot-original files had their copyright headers removed and replaced with "Copyright © IQ.Lvbs, apart of [private], All Rights Reserved." Examples:  
- sunnypilot/__init__.py → iqpilot/__init__.py (95% identical, header replaced)  
- sunnypilot/selfdrive/ui/quiet_mode.py → iqpilot/selfdrive/ui/quiet_mode.py (100% identical, header replaced)  
- sunnypilot/livedelay/helpers.py → iqpilot/livedelay/helpers.py (100% identical, header replaced)  
- sunnypilot/mapd/ entire directory (9 files) → iqpilot/mapd/ (85-98% identical, all headers replaced)  
- sunnypilot/selfdrive/controls/lib/speed_limit/ (5 files) → iqpilot equivalent (75-88% identical)  
  
MECHANICAL STRING REPLACEMENT: 53+ files with 73-100% similarity. "sunnypilot" replaced with "iqpilot", "sunnylink" with "konn3kt", class suffixes "SP" with "IQ" prefix:  
- sunnypilot/modeld_v2/parse_model_outputs.py → iqpilot/modeld_v2/parse_model_outputs.py (100% identical except namespace)  
- sunnypilot/sunnylink/backups/manager.py → iqpilot/konn3kt/backups/manager.py (BackupManagerSP → BackupManagerK3)  
- sunnypilot/sunnylink/backups/AESCipher.py → iqpilot/konn3kt/backups/crypto.py (file renamed, identical AES implementation, class name AESCipher preserved, copyright header replaced)  
- sunnypilot/selfdrive/controls/lib/nnlc/model.py → iqpilot/.../neural_network_feed_forward/network.py (file renamed, same NNTorqueModel class, header replaced)  
- sunnypilot/selfdrive/controls/lib/nnlc/nnlc.py → iqpilot/.../neural_network_feed_forward/nnff.py (file renamed, header replaced)  
- sunnypilot/selfdrive/controls/lib/nnlc/helpers.py → iqpilot/.../neural_network_feed_forward/locator.py (file renamed, header replaced)  
  
CAP'N PROTO SCHEMA COPYING: 15 schema IDs in cereal/custom.capnp are identical between repos but with struct names changed. These hex IDs are unique identifiers generated once — identical IDs prove direct copying:  
- CarParamsSP @0x80ae746ee2596b11 → IQCarParams @0x80ae746ee2596b11  
- SelfdriveStateSP @0x81c2f05a394cf4af → IQState @0x81c2f05a394cf4af  
- BackupManagerSP @0xf98d843bfd7004a3 → IQBackupManager @0xf98d843bfd7004a3  
(and 12 more)  
  
LICENSE REPLACEMENT: sunnypilot's custom MIT License (Copyright 2024 [private], SUNNYPILOT LLC) replaced entirely with proprietary "IQ.Lvbs License v0.1a" (Copyright 2026 IQ.Lvbs LLC) claiming original ownership.  
  
GIT HISTORY ERASURE: All 7 branches contain exactly 1 commit each — systematic history squashing to destroy attribution. Zero remaining references to "sunnypilot" or "[private]" in the repository.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://github.com/sunnypilot/sunnypilot  
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
The entire repository is infringing  
  
**Identify the full repository URL that is infringing:**  
  
https://github.com/IQLvbs/openpilot  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
https://github.com/IQLvbs/openpilot  
  
**Is the work licensed under an open source license?**  
  
Yes  
  
**Which license?**  
  
mit-license  
  
**How do you believe the license is being violated?**  
  
The sunnypilot custom MIT License (Copyright (c) 2024, [private], SUNNYPILOT LLC) has four conditions. Three have been violated:  
  
1. LICENSE CONDITION §1 — PERMISSION REQUIRED: "Permission Required for Commercial, For-Profit, or Closed Source Use: Use of the Software, in whole or in part, for any commercial purposes, for-profit projects, or in closed source projects requires explicit written permission from the original author(s)." The infringer never obtained written permission. Furthermore, they replaced the sunnypilot license entirely with a proprietary "IQ.Lvbs License v0.1a" that claims original ownership of derivative work and restricts others from using code authored by [private], SUNNYPILOT LLC, and contributors.  
  
2. LICENSE CONDITION §2 — REDISTRIBUTION: "Any redistribution of the Software, modified or unmodified, must retain this license notice and the following acknowledgment: 'This software is licensed under a custom license requiring permission for use.'" The infringer removed the license entirely and replaced it with their own. They also removed all per-file copyright headers ("Copyright (c) 2021-, [private], sunnypilot, and a number of other contributors") from 39+ files and replaced them with "Copyright © IQ.Lvbs, apart of [private], All Rights Reserved."  
  
3. LICENSE CONDITION §3 — VISIBILITY: "Any project that uses the Software must visibly mention the following acknowledgment: 'This project uses software from [private] and SUNNYPILOT LLC and is licensed under a custom license requiring permission for use.'" The infringer removed all mentions of [private], SUNNYPILOT LLC, and contributors from the entire codebase and replaced them with IQ.Lvbs/[private] branding. No acknowledgment exists anywhere in the repository.  
  
Additionally, the infringer squashed all git history to single commits across all 7 branches to destroy provenance, and created a fresh repository (not a GitHub fork) to avoid showing the relationship to sunnypilot.  
  
The infringer was directly notified of the license requirements on February 25, 2026 via Discord. The primary operator ("Teal") acknowledged using sunnypilot code (referred to it as "your code") but explicitly stated "ill be removing all sunnypilot references" rather than adding the required attribution. When offered generous compliance terms (just add attribution to README and LICENSE.md), Teal responded "it'll be done today, I don't want to be called 'just another fork of sunnypilot'." Four days later, an associate sent a message falsely claiming "We share no SunnyPilot code and are a direct fork of upstream."  
  
**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**  
  
The infringement is too extensive for simple compliance fixes. The entire codebase has been systematically rebranded — copyright headers replaced in 39+ files, namespace strings replaced in 53+ files, license replaced, git history destroyed. However, if compliance were attempted, it would require at minimum:  
  
1. Restore the per-file copyright headers: "Copyright (c) 2021-, [private], sunnypilot, and a number of other contributors. This file is part of sunnypilot and is licensed under the MIT License. See the LICENSE.md file in the root directory for more details." on all files derived from sunnypilot  
2. Restore the sunnypilot custom MIT License (Copyright (c) 2024, [private], SUNNYPILOT LLC) as LICENSE.md and remove the false "IQ.Lvbs License v0.1a" that claims original ownership of derivative work  
3. Per license condition §2 (Redistribution): retain the license notice and include the acknowledgment: "This software is licensed under a custom license requiring permission for use."  
4. Per license condition §3 (Visibility): visibly mention in the project: "This project uses software from [private] and SUNNYPILOT LLC and is licensed under a custom license requiring permission for use."  
5. Per license condition §1 (Permission Required): obtain explicit written permission from the original author(s) for any commercial, for-profit, or closed-source use by contacting sunnypilot [private]  
6. Remove all false copyright claims attributing sunnypilot-original code (authored by [private], SUNNYPILOT LLC, and contributors) to "IQ.Lvbs, [private]"  
  
Given the scale and deliberate nature of the infringement — systematic history squashing, comprehensive scrubbing of all sunnypilot references, false copyright claims, explicit refusal to comply when asked (Feb 25, 2026), and subsequent false denial of using sunnypilot code — the appropriate remedy is removal of the repository.  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
GitHub username: IQLvbs  
GitHub repository: https://github.com/IQLvbs/openpilot  
Self-hosted [private] mirror: [private]  
Associated websites: [private]  
Contact emails: [private]  
Entity names used in copyright headers and license: [private]  
Jurisdiction claimed in their license: [private]  
  
Note: The infringing code is also hosted on a self-hosted [private] instance at [private], which is operated from the infringer's personal server. This DMCA notice covers the GitHub-hosted repository; the self-hosted mirror is noted for completeness.  
  
Known individuals:  
- [private] user "[private]" — primary [private]/[private] of the [private] project and repository. Previously discussed license compliance in the sunnypilot [private] server, acknowledged the code relationship, then chose to strip all attribution instead of complying. The entity name "[private]." directly references this individual.  
- [private] user "[private]" (username: [private]) — associate/spokesperson for [private].  
  
On February 25, 2026, the primary operator "[private]" sent a direct message acknowledging using sunnypilot code, stating "ill be removing all sunnypilot references" and "I don't want to be called 'just another fork of sunnypilot'." On March 1, 2026, [private] sent an unsolicited direct message falsely claiming "We are now no longer affiliated with SunnyPilot in any capacity. We share no SunnyPilot code and are a direct fork of upstream" — directly contradicting [private]'s acknowledgment four days prior. On March 7, 2026, [private] sent the unsolicited message "You're going to regret that."  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]  
