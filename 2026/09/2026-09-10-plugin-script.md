**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**  
  
Yes, I am the copyright holder.  
  
**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**  
  
No  
  
**Does your claim involve content on GitHub or npm.js?**  
  
GitHub  
  
**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**  
  
I am the [private] and copyright owner of the ZSU and KSU source code. ZSU and KSU is two commercial SketchUp plugin that [private] developed and sell. I have never licensed, open-sourced, or authorized anyone to publish or redistribute its source code.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
I am the [private] and copyright owner of two commercial closed-source SketchUp extensions. The complete original source of both is held in [private] own private [private] repositories, so GitHub can verify every claim below directly:  
  
1. ZSU - a Ruby/C++ extension for parametric furniture modelling in SketchUp (hinge placement, tenon/mortise joinery, tongue-and-groove joints, edge banding, LED groove routing, frame punching, panel splicing, cabinet-door generation, board thickness editing, CNC tool relief, bending/unfolding, restore). Product page: https://xsu.vn/#zsu. Original private source repository: [private] - first commit 23 October 2025, 660 commits to date.  
  
2. KSU - a Ruby/C++ extension that post-processes CNC nesting layouts for furniture panel manufacturing (panel packing and optimisation, anti-fly layer marking, board numbering, 3D labelling, label printing, final-cut lines, DXF export, cost statistics). Sold at https://ksu.vn, product page https://xsu.vn/#ksu. Original private source repository: [private] - first commit 27 May 2026, 341 commits to date.  
  
Both products are distributed to paying customers only as encrypted, licence-activated .rbz packages. Neither repository has ever been made public, open-sourced, or licensed to anyone for redistribution, modification or derivative works. [private] will grant GitHub staff read access to either repository on request.  
  
The material identified in this notice - the package zns_1.1.30.rbz, distributed under the name "ZNS" / "Zenith System", whose own zns.rb loader falsely describes it as "Bộ công cụ mã nguồn mở" ("open-source toolkit") while shipping every Ruby file encrypted as .rbe - is an unauthorised derivative work created by merging and renaming [private] two products.  
  
DIRECT COMPARISON  
  
Every file path cited in sections A-H below is a path in one of the two private repositories named above, on branch main. The file pairs are:  
  
[private]  
  
One pair is sufficient to decide this notice: [private] against [private]. GitHub does not need to review the full commit history of either repository.  
  
The copied JavaScript is not recent work that could be claimed in the other direction. [private] was first committed to [private] private repository on 14 January 2026 (commit ffb2bf1) and carries 36 commits of authorship history, most recently a469e8d of 29 August 2026. [private] was first committed on 27 May 2026 (commit 36a6490). The infringing binary that reproduces them was compiled on 2 September 2026 and uploaded the same day.  
  
EVIDENCE OF COPYING  
  
A. The settings framework is [private] file, de-branded.  
  
[private] [private] and the package's [private] both open with a global named PRESET_PANEL_HTML, built by the same string-concatenation of the same Vue markup. Removing only the cosmetic "zns-" class prefix and whitespace, 23 of the first 26 lines are identical character for character - the same attribute order, the same Vue directives [private]. The only three differing lines differ solely because three hard-coded [private] literals of mine were replaced by translation variables - and one of those literals, [private], still survives verbatim as the fallback value at line 101 of the package's own file.  
  
B. Identical CSS class vocabulary.  
  
The preset panel uses exactly 27 class names in [private] source; the package uses exactly the same 27, none added and none omitted: preset-container, preset-form, preset-input-wrap, preset-input, preset-button-group, preset-button, preset-button-save, preset-button-delete, preset-list, preset-list-empty, preset-panel, preset-group-header, preset-group-label, preset-group-name, preset-group-toggle, preset-item, preset-item-text, preset-item-active, preset-item-active-disabled, preset-item-disabled, preset-item-grouped, preset-item-grouped-last, preset-checkbox, dragging-active, drag-gap, drag-gap-end, search-clear.  
  
C. Identical method sequence and bodies.  
  
Both files declare the same 30 methods in the same consecutive order: [private], a functionless artefact that exists in [private] code only for backward compatibility and that no independently written code would reproduce.  
  
D. [private] [private]-language internal key convention was retained.  
  
[private]
  
E. [private] private host-bridge protocol.  
  
All ten postMessage action names of [private] settings bridge - save_setting, save_preset, load_preset, load_presets, delete_preset, save_presets_order, save_collapsed_groups, reset_section, iframe_ready, search_index - appear identically in the package. Of the 32 sketchup.* callbacks its UI invokes, 17 are exactly the callback names registered in [private] Ruby sources: save_setting, load_settings, reset_settings, reset_section, save_collapsed_groups, pick_file, save_preset, load_preset, delete_preset, save_presets_order, load_presets, import_settings, export_settings, import_online, export_online, check_update_version, uninstall.  
  
F. Settings-shell internals.  
  
The private JavaScript embedded in [private] [private] and the package's [private] share the same internal names, including loadSettingsToIframe(tabId), iframesReady, pendingLoads, currentTabId, searchIndex, and the 'iframe_' + tabId element-id scheme.  
  
G. 259 verbatim namese interface strings.  
  
259 interface strings from [private] two source trees appear in the package's [private] - 135 traceable to ZSU and 124 to KSU - including complete sentences reproduced character for character with identical punctuation:  
  
[private]  
  
These are idiosyncratic [private] cabinetmaking terms and full help sentences of [private] own authorship, not generic user-interface vocabulary.  
  
H. The feature set is [private] two products, renamed into English.  
  
Every tool directory in the package maps one-to-one onto a tool of mine.  
  
From ZSU: [private]  
  
From KSU: [private]  
  
The package even ships [private], a compatibility shim for the same third-party plugin that [private] own products patch.  
  
CIRCUMVENTION  
  
The package additionally circumvents the licence-activation mechanism that protects [private] works, so that software normally requiring a paid activation runs without one.  
  
REPEAT INFRINGEMENT  
  
This is the same work, by the same GitHub account, that GitHub already disabled following [private] notice of 25 June 2026 (ticket [private], published at [private]). The two repositories named in that notice, [private] and [private], remain access-blocked.  
  
The account has now created a new, deliberately unnamed repository - hvtohl/f, created 2 September 2026, with no description and a single placeholder file - whose only real content is the successor build zns_1.1.30.rbz, published twice under two separate release tags, together with a version.json update manifest. This re-establishes the auto-update channel that the June takedown removed.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://xsu.vn/#ksu (official commercial KSU product page)  
https://xsu.vn/#zsu (official commercial ZSU product page)  
  
Original private source repository: [private]  
Original private source repository: [private]  
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
The entire repository is infringing  
  
**Identify the full repository URL that is infringing:**  
  
https://github.com/hvtohl/f  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
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
