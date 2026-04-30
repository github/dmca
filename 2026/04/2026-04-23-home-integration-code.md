Before disabling any content in relation to this takedown notice, GitHub  
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).  
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).  
  
To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).  
  
---  
  
**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**  
  
Yes, I am the copyright holder.  
  
**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**  
  
Yes  
  
**Please provide the Zendesk ticket number of your previously submitted notice. Zendesk ticket numbers are 7 digit ID numbers located in the subject line or body of your confirmation email.**  
  
4292263  
  
**Does your claim involve content on GitHub or npm.js?**  
  
GitHub  
  
**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**  
  
I am the [private] and copyright owner of the source code in the repositories listed below. The code was created and published by [private] on GitHub under [private] account "cnecrea". Four of the five repositories (eonromania, hidroelectrica, myelectrica, vreaulanova) are licensed under the MIT License, which requires that [private] copyright notice be preserved in all copies or substantial portions of the software. The fifth repository (e-bloc.ro) has no license file and is therefore under default "all rights reserved" copyright. No license was granted to any third party to strip attribution, relicense the code under their own name, or commercially redistribute the work without compliance.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
The copyrighted work consists of multiple Home Assistant integrations developed by [private], providing functionality for interacting with Romanian utility providers (E.ON, Hidroelectrica, Electrica / myElectrica, Nova, and e-Bloc). These integrations include custom Python code, API communication logic, authentication handling, data parsing, and entity management specific to each provider, along with Romanian-language user-facing strings, translation files, and distinctive architectural design — including a two-phase refresh coordinator, a server-side licensing module with Ed25519/HMAC-SHA256 validation, automatic prosumer detection via the "1.8.0_P" register, automatic meter-reading window detection, and a "Trimite index" submission flow.  
  
The infringing repository (https://github.com/mariusonitiu/utilitati_romania) has copied substantial portions of this code and combined them into a single aggregated integration, without permission, attribution, or license compliance.  
  
EVIDENCE OF INFRINGEMENT:  
  
(1) LICENSE FILE COMPARISON. [private] LICENSE file at https://github.com/cnecrea/hidroelectrica/blob/main/LICENSE reads: "Licență MIT — Copyright (c) 2024 [cnecrea]". The same notice appears in the LICENSE files of eonromania, myelectrica, and vreaulanova. The infringing LICENSE at https://github.com/mariusonitiu/utilitati_romania/blob/main/LICENSE reads: "MIT License — Copyright (c) 2026 mariusonitiu". [private] required copyright notice has been replaced, not preserved. This is a direct and verifiable violation of the MIT License attribution requirement.  
  
(2) FEATURE-SET AND PROVIDER SCOPE. The infringing repository's list of supported providers matches [private] individual repositories one-to-one: E.ON (cnecrea/eonromania), Hidroelectrica (cnecrea/hidroelectrica), myElectrica (cnecrea/myelectrica), Nova (cnecrea/vreaulanova), eBloc (cnecrea/e-bloc.ro). Distinctive features specific to [private] implementation also appear in the infringing repository: server-side licensing system advertised as "trial 90 zile + lifetime", "Trimite index" meter-reading submission, automatic reading-window detection, and a Romanian-language user interface.  
  
(3) NO ATTRIBUTION. The README, documentation, and source files of the infringing repository present the project as the infringer's original work. [private] name, [private] GitHub username (cnecrea), and links to [private] upstream repositories appear nowhere. The repository is not a GitHub fork of [private] repositories.  
  
(4) CHRONOLOGY. [private] repositories were published in 2024. For example, cnecrea/hidroelectrica has 96 stars, 8 forks, and public issues dating from January 2025. The infringing repository's copyright is dated 2026 and the repository has 23 total commits at the time of this notice, with 1 star and 0 forks.  
  
(5) COMMERCIAL EXPLOITATION. The infringing repository monetizes the copied code through an integrated licensing system advertised as "trial 90 zile + lifetime upgrade" and solicits donations via [private], while simultaneously stripping the attribution required by the license under which the code was originally released.  
  
(6) E-BLOC.RO. [private] repository cnecrea/e-bloc.ro has no LICENSE file and is therefore all-rights-reserved under default copyright. Code derived from this repository has been reused in the infringing repository without any license or permission of any kind — this is a direct copyright infringement independent of any license terms.  
  
TO BRING THE PROJECT INTO COMPLIANCE, the infringer would need to: (a) restore "Copyright (c) 2024 [cnecrea]" in the LICENSE file, alongside any legitimate contributions of his own; (b) add clear attribution in the README stating that substantial portions are derived from [private] upstream repositories, with links; (c) restore any copyright headers removed from source files; and (d) remove any code derived from cnecrea/e-bloc.ro, for which no license was ever granted.  
  
NOTE ON PRIOR NOTICE: In [private] original notice on ticket #4292263, I answered "No" to the question about open-source licensing. That answer was a clerical error made while tired. The correct answer is "Yes" for four repositories (MIT License) and "No license" for one (e-bloc.ro). This revised, complete notice supersedes the previous one in full.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://github.com/cnecrea/eonromania    
https://github.com/cnecrea/hidroelectrica    
https://github.com/cnecrea/myelectrica    
https://github.com/cnecrea/vreaulanova    
https://github.com/cnecrea/e-bloc.ro    
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
The entire repository is infringing  
  
**Identify the full repository URL that is infringing:**  
  
https://github.com/mariusonitiu/utilitati_romania  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
**Is the work licensed under an open source license?**  
  
Yes  
  
**Which license?**  
  
mit-license  
  
**How do you believe the license is being violated?**  
  
The MIT License explicitly requires that "the above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software."  
  
The infringer has copied substantial portions of [private] MIT-licensed source code (from cnecrea/eonromania, cnecrea/hidroelectrica, cnecrea/myelectrica, and cnecrea/vreaulanova) into the reported repository, but has NOT preserved [private] copyright notice as required.  
  
[private] LICENSE file at https://github.com/cnecrea/hidroelectrica/blob/main/LICENSE reads:  
"Licență MIT — Copyright (c) 2024 [cnecrea]"  
  
The infringing LICENSE file at https://github.com/mariusonitiu/utilitati_romania/blob/main/LICENSE reads:  
"MIT License — Copyright (c) 2026 mariusonitiu"  
  
[private] required copyright notice has been replaced with the infringer's own name, and the year has been changed from 2024 (original publication) to 2026, falsely asserting primary authorship. This is a direct and verifiable violation of the attribution requirement of the MIT License.  
  
The infringing repository additionally presents the project as the infringer's original work in its README and documentation — with no attribution to [private], no link to [private] upstream repositories, and no indication that this is derivative work. It also monetizes the copied code via an integrated "trial 90 zile + lifetime" licensing system and donation solicitation at [private], while stripping the attribution that the MIT License required in exchange for the permission to reuse.  
  
**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**  
  
To bring the project into compliance with the MIT License, the infringer would need to make the following changes:  
  
1. Restore [private] original copyright notice in the LICENSE file. Specifically, add "Copyright (c) 2024 [cnecrea]" to the LICENSE file. This can coexist with any legitimate copyright claim the infringer wishes to assert over his own contributions, but [private] original notice must be preserved as required by the MIT License.  
  
2. Restore any copyright headers that were removed from individual source files (the .py files inside the custom_components directory) where they existed in [private] original repositories.  
  
3. Add clear attribution in the README stating that substantial portions of the code are derived from [private] upstream repositories, with direct links:  
- https://github.com/cnecrea/eonromania  
- https://github.com/cnecrea/hidroelectrica  
- https://github.com/cnecrea/myelectrica  
- https://github.com/cnecrea/vreaulanova  
  
4. Remove any code derived from https://github.com/cnecrea/e-bloc.ro, which has no LICENSE file and is therefore all-rights-reserved under default copyright — no license or permission of any kind was ever granted to reuse that code.  
  
Alternatively, if the infringer is unwilling to restore the required attribution and remove the unlicensed code, the repository should be made private or taken down in full.  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]  
