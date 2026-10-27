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
  
I am the [private] and copyright holder of the software project "yandex-music-streamdeck" (Copyright (c) 2025 Judd1zzz), including the file injected_api.js at issue. [private] wrote this code [private] and published it on GitHub under the MIT License. I am submitting this notice on [private] behalf as an [private].  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
The original work is "yandex-music-streamdeck", a Stream Deck / StreamDock plugin that controls the Yandex Music desktop client via the Chrome DevTools Protocol (CDP). Its core component is the file injected_api.js — a JavaScript controller injected into the player page. It locates player UI elements through a layered selector map (DOM.SCOPE / Track / Controls / Volume), exposes playback commands (playPause, next, prev, toggleLike, changeVolume, etc.), and continuously observes the player state, pushing updates to the plugin backend through a CDP binding named "sdNotify" using a FULL_STATE / DELTA protocol computed by a recursive deepDiff function.  
  
The work is published under the MIT License with the copyright notice  
"Copyright (c) 2025 Judd1zzz".  
  
**If the original work referenced above is available online, please provide a URL.**  
  
Repository: https://github.com/Judd1zzz/yandex-music-streamdeck  
  
The copied file in its original form (permalink; this revision predates the creation of the infringing repository):  
https://github.com/Judd1zzz/yandex-music-streamdeck/blob/2b711d7a201f7b0d6d33f16faeba59541df05d13/com.judd1.yandex_music.sdPlugin/src/core/scripts/injected_api.js  
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
Specific files within the repository are infringing  
  
**Identify only the specific file URLs within the repository that is infringing:**  
  
The infringing file (specific file, not the entire repository):  
https://github.com/Vallhant/Spotify-Music-Ajazz-Plugin/blob/main/plugin/scripts/injected_api.js  
  
This file is a derivative adaptation of [private] injected_api.js: the DOM selectors were swapped from Yandex Music to Spotify, while the architecture, control flow, data structures, wire protocol and [private] project-specific identifiers were retained. 99 of the 448 meaningful lines (~22%) are verbatim identical to [private] file after normalizing whitespace and quote style, and entire functions are copied verbatim or near-verbatim. Line references below are "[private] file (permalink above) -> infringing file (main branch)":  
  
- Utils.find / Utils.findBtn: lines 32-45 -> lines 82-96 (near-verbatim; identical bodies, e.g. lines 37-41 -> 87-91 verbatim).  
- Utils.toPercent: lines 82-88 -> lines 197-203 (verbatim, including the 0..1-vs-percent heuristic "if (n >= 0 && n <= 1) return Math.round(n*100); return Math.min(Math.round(n), 100);").  
- Element cache helpers _findOne / _findBtnOne: lines 106-122 -> lines 219-231 (verbatim, including the "this.cache[key].isConnected" caching scheme; lines 110-115 -> 221-226 and 119-124 -> 228-233 are identical).  
- getFullState skeleton: lines 128-134 -> lines 265-272, including [private] error code "BAR_NOT_FOUND" (line 131 -> line 269), and the identical response shape { success, data: { track, state, progress, volume } }: lines 143-161 -> lines 289-306.  
- playPause with the "is_playing" response key: lines 164-180 -> 309-329.  
- changeVolume command switch (UP/DOWN/SET/MUTE, step = 0.05): lines 203-219 -> lines 375-418.  
- deepDiff recursive state-diff algorithm: lines 337-370 -> lines 592-606 (same algorithm and semantics; parameters renamed).  
- startObservation polling loop with [private] FULL_STATE / DELTA protocol: lines 372-400 -> lines 608-631 (near-verbatim; only a variable name and the polling interval differ), followed by stopObservation copied verbatim: lines 399-405 -> lines 630-636.  
- _notify emitting JSON through [private] CDP binding name "sdNotify": lines 418-426 -> lines 637-640.  
- File epilogue (controller instantiation, window global, autostart): lines 429-435 -> lines 643-647.  
  
The identifiers sdNotify, FULL_STATE, DELTA, BAR_NOT_FOUND, now_sec, total_sec, ratio, is_playing, new_state, _findOne, _findBtnOne, _clickSimple, _getProgressState, _getVolumeState are [private] original naming choices carried over unchanged; they are not dictated by any external API.  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
**Is the work licensed under an open source license?**  
  
Yes  
  
**Which license?**  
  
mit-license  
  
**How do you believe the license is being violated?**  
  
The MIT License requires that "the above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software." The infringing repository reproduces substantial portions of [private] work (see the file identification above) but contains no copyright notice, no copy of the MIT license text, and no attribution of any kind, in any file or in the README. The use is therefore outside the scope of the license and unauthorized.  
  
**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**  
  
Adding [private] copyright notice "Copyright (c) 2025 Judd1zzz", the full MIT license text, and a visible attribution to the original project (https://github.com/Judd1zzz/yandex-music-streamdeck) would bring the use into compliance with the MIT License. Alternatively, removing the file plugin/scripts/injected_api.js or making the repository private would resolve the infringement. The owner has not responded to [private] attribution request for three weeks, so if they remain unresponsive, please disable the repository.  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
Only the GitHub username: Vallhant (https://github.com/Vallhant). I have no other contact information. [private] attempted to contact them on July 4, 2026 via an issue in the infringing repository ([private]), asking only for attribution and inclusion of the license. There has been no response for three weeks.  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]  
