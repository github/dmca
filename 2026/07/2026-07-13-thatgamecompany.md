Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I have read and understand GitHub's Guide to Filing a DMCA Notice.  
We, thatgamecompany, inc., are the developer and publisher of the video game Sky: Children of the Light and the owner of the copyrights in the game's client software, source code, compiled binaries, game assets (art, models, animations, audio, music, UI), and associated proprietary materials. I am authorized to act on behalf of thatgamecompany, inc. in submitting this notice and in matters relating to the protection of its intellectual property on third-party platforms.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The copyrighted works at issue are the proprietary client software and creative content comprising Sky: Children of the Light ("Sky"), a multi-platform online social adventure game first released by thatgamecompany, inc. in 2019 and continuously developed and updated since. The protected works include, without limitation: the Sky game client executable and supporting binaries; proprietary game logic, scripts, and networking code; copyrighted art assets (characters, environments, cosmetics, dyes, spells, animations); copyrighted audio and musical compositions (including the in-game instrument and music systems); user interface elements; and the data structures, identifiers, asset names, and protocol definitions used by the client to communicate with thatgamecompany's authoritative game servers. The current Sky client version referenced by the infringing project is v0.33.7.394009.

**If the original work referenced above is available online, please provide a URL.**

https://thatskygame.com  
https://store.steampowered.com/app/2325290/Sky_Children_of_the_Light/  
https://apps.apple.com/us/app/sky-children-of-the-light/id1462117269  
https://play.google.com/store/apps/details?id=com.tgc.sky.android

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/XeTrinityz/ThatSkyMod

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

thatgamecompany employs multiple technological protection measures ("TPMs") that effectively control access to the copyrighted Sky client and its protected runtime content:

- Authenticated session and account access controls. The Sky client requires a valid, authenticated account session with thatgamecompany's servers in order to load, decrypt, and access protected game content and progression state. Unauthenticated processes cannot lawfully access this content.  
- Server-authoritative game state. Player position, inventory, currency (candles, hearts, ascended candles, wax), cosmetic unlocks, friendship/affinity state, session membership, and progression are validated and enforced by thatgamecompany's authoritative servers. Access to, and modification of, this protected state is restricted to communications issued by an unmodified, authenticated client.  
- Client integrity and anti-tamper protections. The Sky client is distributed as a compiled binary with integrity checks intended to prevent unauthorized code injection, in-process hooking, and runtime modification of game logic and memory.  
- Protocol and asset protection. Internal asset identifiers, network message formats, and proprietary data tables (cosmetic keys, spell IDs, quest definitions, shop entries) are non-public and are accessible only through the licensed client's authorized code paths.
- EULA-gated licensed access. End users obtain access to the protected client only by accepting the Sky End User License Agreement, which conditions access on the prohibition of reverse engineering, modification, decompilation, automation, and the use of unauthorized third-party software.

Together, these measures effectively control access to the copyrighted Sky client software and the copyrighted runtime content the client renders, by limiting access to authorized users running an unmodified client in an authenticated session.

**How is the accused project designed to circumvent your technological protection measures?**

The "ThatSkyMod" project ("TSM") is purpose-built to defeat the technological measures described above. Specifically, the project:

- Injects unauthorized code into the running Sky client process, hooking internal functions and modifying client behavior in memory in order to bypass client integrity protections.  
- Reads, parses, and exposes protected internal identifiers and data structures — including cosmetic keys, spell/outfit/stat names, asset names, quest definitions, and shop entries — that the TPMs are designed to keep accessible only to authorized client code paths (see, e.g., the project's release notes referencing "Unlocks → Shop [API]," "Outfit, Spells & Stat Names," "Memory Scan," and "ImGui Inspect" features).  
- Bypasses server-authoritative state controls by issuing client actions that the legitimate client would not permit — including teleportation to arbitrary world coordinates and to other players ("Warp to Player," "Teleport to Krill/Crab/Butterfly/Bird," "Free Camera" teleport), invincibility ("Armor Immunity"), unauthorized currency and progression manipulation ("Network Wax Run," "Collect All Dyes," "Buy All Free Items," "Unlock by Key," "Acknowledge Unlocks"), session manipulation ("Prevent Session Split," "Disable Joinable," "Join Session," "Spectate"), and entity control over networked creatures and NPCs.  
- Circumvents the licensed client's intended music and instrument systems by extracting and locally storing copyrighted in-game music ("Game music is now automatically detected and stored locally") and by driving the instrument system through an automated sheet player.  
- Modifies the protected rendering and environment pipeline of the licensed client (ESP overlays, "Camera Through Walls," "Disable Environment," "Draw Distance," fog and sun parameters, "Bone Skeleton," "Krill ESP") to reveal content and spatial information the TPMs and authorized client are designed to gate.
- Defeats EULA-gated licensed access by providing a comprehensive toolkit whose stated purpose, per the repository's own README, is to give users "maximum freedom" and "complete player freedom" over a client whose access is conditioned on the prohibition of exactly these modifications.

The project's own release notes and feature list (including but not limited to releases v0.30.0 through v0.37.0) document, in the project owner's own words, that TSM is designed to operate against, and to defeat, the protections described above.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/TOM-231-face/ThatSkyMod  
https://github.com/sguzman/ThatSkyMod  
https://github.com/knightofmaster/ThatSkyMod  
https://github.com/adasd121211/ThatSkyMod  
https://github.com/yxsj245/ThatSkyMod

**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

The only contact information known to us is the GitHub account associated with the repository:

- GitHub username: XeTrinityz ([private])  
- Project [private] server linked from the repository [private]

Additional contributors listed in the repository README include the GitHub-visible handles: [private], [private], [private], [private], [private], among others. We do not have direct off-platform contact information for these individuals.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
