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
  
I am the [private] and [private] copyright holder of the open-source software project "Hybrid Mount". [private] authored the source code, and [private] ownership is verifiable through the project's Git commit history and the original repository's metadata.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
The copyrighted work is [private] comprehensive open-source software project titled "Hybrid Mount" (Internal Package Name: meta-hybrid).  
  
Detailed Description of the Work: The work is a specialized system utility designed for Android filesystem management, consisting of multi integral components that function as a unified system:  
  
The Architectural Design & Project Structure: The unique file organization, module division (e.g., specific separation of core logic, and inventory modules), and workflow logic which constitutes the non-literal elements of the software copyright.  
  
The Web User Interface (WebUI): A distinct frontend application written in TypeScript/JavaScript and Svelte. This component includes specific UI layouts, styling (CSS), and component logic that are unique to [private] creation.  
  
The Project Identity & History: The work is defined by its continuous development history. The original repository contains the complete Git commit log dating back to the project's inception, which serves as the forensic proof of [private] authorship for the entire project lifecycle.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://github.com/YuzakiKokuban/meta-hybrid_mount  
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
The entire repository is infringing  
  
**Identify the full repository URL that is infringing:**  
  
https://github.com/anatdx/hymo  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
https://github.com/anatdx/hymo  
  
**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**  
  
**Is the work licensed under an open source license?**  
  
Yes  
  
**Which license?**  
  
gnu-affero-general-public-license-v3.0  
  
**How do you believe the license is being violated?**  
  
The repository violates the GNU General Public License v3.0 (GPLv3) in the following ways:  
  
Violation of Attribution Requirements (Section 5(a)): The GPLv3 license permits modification and redistribution of source code only if the modified work carries "prominent notices stating that you modified it". The infringing repository "hymo" is a derivative work of [private] project "Hybrid Mount" (as proven by the retained Git commit history and identical WebUI code structure). However, the user has failed to provide appropriate attribution to the original project and author.  
  
Removal of Copyright Notices (Section 4 & 5): The license requires that all copyright notices be preserved. The user has willfully stripped [private] copyright notices and project references. Specifically, the commit with the message "chore: 移除对 Meta-Hybrid Mount 的引用" (Translation: "chore: remove references to Meta-Hybrid Mount") [https://github.com/Anatdx/hymo/commit/52dac8883a25138575b56b60066a77d720d81bab]provides direct evidence of the user's intent to remove Copyright Management Information (CMI) and conceal the software's origin, which is a direct violation of the license terms.  
  
Unlawful Redistribution: Because the conditions of the GPLv3 (attribution and preservation of notices) have not been met, the user's rights to propagate and modify the work are automatically terminated under Section 8. Therefore, the current distribution of the repository is unlicensed and infringing.  
  
Hybird Mount Tree:  
.  
├── ./Cargo.lock  
├── ./Cargo.toml  
├── ./changelog.md  
├── ./icon.svg  
├── ./LICENSE  
├── ./module  
│   ├── ./module/config.toml  
│   ├── ./module/customize.sh  
│   ├── ./module/metainstall.sh  
│   ├── ./module/metamount.sh  
│   ├── ./module/metauninstall.sh  
│   ├── ./module/module.prop  
│   ├── ./module/tools  
│   │   └── ./module/tools/mkfs.erofs  
│   └── ./module/uninstall.sh  
├── ./module.json  
├── ./notify  
├── ./README_ZH.md  
├── ./README.md  
├── ./src  
│   ├── ./src/conf  
│   │   ├── ./src/conf/cli_handlers.rs  
│   │   ├── ./src/conf/cli.rs  
│   │   ├── ./src/conf/config.rs  
│   │   └── ./src/conf/mod.rs  
│   ├── ./src/core  
│   │   ├── ./src/core/executor.rs  
│   │   ├── ./src/core/granary.rs  
│   │   ├── ./src/core/inventory.rs  
│   │   ├── ./src/core/mod.rs  
│   │   ├── ./src/core/modules.rs  
│   │   ├── ./src/core/planner.rs  
│   │   ├── ./src/core/state.rs  
│   │   ├── ./src/core/storage.rs  
│   │   ├── ./src/core/sync.rs  
│   │   └── ./src/core/winnow.rs  
│   ├── ./src/defs.rs  
│   ├── ./src/main.rs  
│   ├── ./src/mount  
│   │   ├── ./src/mount/magic.rs  
│   │   ├── ./src/mount/mod.rs  
│   │   ├── ./src/mount/node.rs  
│   │   └── ./src/mount/overlay.rs  
│   └── ./src/utils.rs  
├── ./tools  
│   └── ./tools/notify  
│   ├── ./tools/notify/Cargo.lock  
│   ├── ./tools/notify/Cargo.toml  
│   └── ./tools/notify/src  
│   └── ./tools/notify/src/main.rs  
├── ./update.json  
├── ./webui  
│   ├── ./webui/index.html  
│   ├── ./webui/package-lock.json  
│   ├── ./webui/package.json  
│   ├── ./webui/pnpm-lock.yaml  
│   ├── ./webui/README.md  
│   ├── ./webui/src  
│   │   ├── ./webui/src/app.css  
│   │   ├── ./webui/src/App.svelte  
│   │   ├── ./webui/src/components  
│   │   │   ├── ./webui/src/components/BottomActions.svelte  
│   │   │   ├── ./webui/src/components/ChipInput.svelte  
│   │   │   ├── ./webui/src/components/NavBar.css  
│   │   │   ├── ./webui/src/components/NavBar.svelte  
│   │   │   ├── ./webui/src/components/Skeleton.css  
│   │   │   ├── ./webui/src/components/Skeleton.svelte  
│   │   │   ├── ./webui/src/components/Toast.css  
│   │   │   ├── ./webui/src/components/Toast.svelte  
│   │   │   ├── ./webui/src/components/TopBar.css  
│   │   │   └── ./webui/src/components/TopBar.svelte  
│   │   ├── ./webui/src/init.ts  
│   │   ├── ./webui/src/layout.css  
│   │   ├── ./webui/src/lib  
│   │   │   ├── ./webui/src/lib/api.mock.ts  
│   │   │   ├── ./webui/src/lib/api.ts  
│   │   │   ├── ./webui/src/lib/constants_gen.ts  
│   │   │   ├── ./webui/src/lib/constants.ts  
│   │   │   ├── ./webui/src/lib/store.svelte.ts  
│   │   │   ├── ./webui/src/lib/theme.ts  
│   │   │   └── ./webui/src/lib/types.ts  
│   │   ├── ./webui/src/locales  
│   │   │   ├── ./webui/src/locales/en.json  
│   │   │   ├── ./webui/src/locales/es.json  
│   │   │   ├── ./webui/src/locales/ja.json  
│   │   │   ├── ./webui/src/locales/ru.json  
│   │   │   ├── ./webui/src/locales/zhs.json  
│   │   │   └── ./webui/src/locales/zht.json  
│   │   ├── ./webui/src/main.ts  
│   │   └── ./webui/src/routes  
│   │   ├── ./webui/src/routes/ConfigTab.css  
│   │   ├── ./webui/src/routes/ConfigTab.svelte  
│   │   ├── ./webui/src/routes/GranaryTab.css  
│   │   ├── ./webui/src/routes/GranaryTab.svelte  
│   │   ├── ./webui/src/routes/InfoTab.css  
│   │   ├── ./webui/src/routes/InfoTab.svelte  
│   │   ├── ./webui/src/routes/LogsTab.css  
│   │   ├── ./webui/src/routes/LogsTab.svelte  
│   │   ├── ./webui/src/routes/ModulesTab.css  
│   │   ├── ./webui/src/routes/ModulesTab.svelte  
│   │   ├── ./webui/src/routes/StatusTab.css  
│   │   ├── ./webui/src/routes/StatusTab.svelte  
│   │   ├── ./webui/src/routes/WinnowingTab.css  
│   │   └── ./webui/src/routes/WinnowingTab.svelte  
│   ├── ./webui/svelte.config.js  
│   ├── ./webui/tsconfig.json  
│   └── ./webui/vite.config.js  
└── ./xtask  
├── ./xtask/Cargo.toml  
└── ./xtask/src  
├── ./xtask/src/main.rs  
└── ./xtask/src/zip_ext.rs  
Hymo Tree:  
.  
├── ./changelog.md  
├── ./docs  
│   └── ./docs/README_ZH.md  
├── ./LICENSE  
├── ./Makefile  
├── ./module  
│   ├── ./module/action.sh  
│   ├── ./module/config.toml  
│   ├── ./module/createimg.sh  
│   ├── ./module/customize.sh  
│   ├── ./module/hot_mount.sh  
│   ├── ./module/hot_unmount.sh  
│   ├── ./module/metainstall.sh  
│   ├── ./module/metamount.sh  
│   ├── ./module/metauninstall.sh  
│   ├── ./module/module.prop  
│   └── ./module/uninstall.sh  
├── ./README.md  
├── ./script  
│   ├── ./script/buildbot.py  
│   └── ./script/sign_zip.py  
├── ./src  
│   ├── ./src/conf  
│   │   ├── ./src/conf/config.cpp  
│   │   └── ./src/conf/config.hpp  
│   ├── ./src/core  
│   │   ├── ./src/core/executor.cpp  
│   │   ├── ./src/core/executor.hpp  
│   │   ├── ./src/core/inventory.cpp  
│   │   ├── ./src/core/inventory.hpp  
│   │   ├── ./src/core/modules.cpp  
│   │   ├── ./src/core/modules.hpp  
│   │   ├── ./src/core/planner.cpp  
│   │   ├── ./src/core/planner.hpp  
│   │   ├── ./src/core/state.cpp  
│   │   ├── ./src/core/state.hpp  
│   │   ├── ./src/core/storage.cpp  
│   │   ├── ./src/core/storage.hpp  
│   │   ├── ./src/core/sync.cpp  
│   │   └── ./src/core/sync.hpp  
│   ├── ./src/defs.hpp  
│   ├── ./src/main.cpp  
│   ├── ./src/mount  
│   │   ├── ./src/mount/hymo_magic.h  
│   │   ├── ./src/mount/hymofs.cpp  
│   │   ├── ./src/mount/hymofs.hpp  
│   │   ├── ./src/mount/magic.cpp  
│   │   ├── ./src/mount/magic.hpp  
│   │   ├── ./src/mount/overlay.cpp  
│   │   └── ./src/mount/overlay.hpp  
│   ├── ./src/utils.cpp  
│   └── ./src/utils.hpp  
├── ./update.json  
└── ./webui  
├── ./webui/index.html  
├── ./webui/jsconfig.json  
├── ./webui/package-lock.json  
├── ./webui/package.json  
├── ./webui/pnpm-lock.yaml  
├── ./webui/src  
│   ├── ./webui/src/app.css  
│   ├── ./webui/src/App.svelte  
│   ├── ./webui/src/components  
│   │   ├── ./webui/src/components/FilePicker.svelte  
│   │   ├── ./webui/src/components/NavBar.css  
│   │   ├── ./webui/src/components/NavBar.svelte  
│   │   ├── ./webui/src/components/Skeleton.svelte  
│   │   └── ./webui/src/components/Toast.svelte  
│   ├── ./webui/src/layout.css  
│   ├── ./webui/src/lib  
│   │   ├── ./webui/src/lib/api.js  
│   │   ├── ./webui/src/lib/api.mock.js  
│   │   ├── ./webui/src/lib/constants_gen.js  
│   │   ├── ./webui/src/lib/constants.js  
│   │   ├── ./webui/src/lib/store.svelte.js  
│   │   └── ./webui/src/lib/theme.js  
│   ├── ./webui/src/locales  
│   │   ├── ./webui/src/locales/ar.json  
│   │   ├── ./webui/src/locales/en.json  
│   │   ├── ./webui/src/locales/es.json  
│   │   ├── ./webui/src/locales/fr.json  
│   │   ├── ./webui/src/locales/ja.json  
│   │   ├── ./webui/src/locales/ru.json  
│   │   ├── ./webui/src/locales/zhs.json  
│   │   └── ./webui/src/locales/zht.json  
│   ├── ./webui/src/main.js  
│   └── ./webui/src/routes  
│   ├── ./webui/src/routes/ConfigTab.css  
│   ├── ./webui/src/routes/ConfigTab.svelte  
│   ├── ./webui/src/routes/InfoTab.css  
│   ├── ./webui/src/routes/InfoTab.svelte  
│   ├── ./webui/src/routes/LogsTab.css  
│   ├── ./webui/src/routes/LogsTab.svelte  
│   ├── ./webui/src/routes/ModulesTab.css  
│   ├── ./webui/src/routes/ModulesTab.svelte  
│   ├── ./webui/src/routes/StatusTab.css  
│   └── ./webui/src/routes/StatusTab.svelte  
├── ./webui/svelte.config.js  
└── ./webui/vite.config.js  
  
**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**  
  
To become compliant with the GPLv3 license, the user must take ALL of the following actions:  
  
Restore Copyright Notices: Restore the original copyright headers and attribution in all source files, especially the WebUI components (which are verbatim copies) and the backend logic derived from [private] work.  
  
Revert Malicious Changes: Revert the commit labeled "chore: 移除对 Meta-Hybrid Mount 的引用", which was explicitly designed to strip [private] authorship information.  
  
Add Prominent Attribution: Include a prominent notice in the README.md and the software's user interface stating that this project is a fork of "Hybrid Mount" and is based on [private] original work, as required by GPLv3 Section 5(a).  
  
Alternatively: If the user is unwilling to provide proper attribution and restore the removed copyright information, they must immediately stop distributing the software by deleting the repository or making it private.  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
I do not have personal contact information for the user.  
  
GitHub Username: anatdx Repository URL: https://github.com/anatdx/hymo Profile URL: https://github.com/anatdx  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]  
