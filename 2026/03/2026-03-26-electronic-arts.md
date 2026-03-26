While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

One or more repositories in this DMCA takedown notice has been processed in accordance with GitHub's prohibition on sharing unauthorized product licensing keys, software for generating unauthorized product licensing keys, and/or software for bypassing checks for product licensing keys.

You can learn more in [GitHub's Acceptable Use Policies](https://docs.github.com/en/github/site-policy/github-acceptable-use-policies).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am [private] for Electronic Arts Inc. (EA). EA is the exclusive copyright licensee of the work and is authorized to act on behalf of the copyright owner.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The infringed works are Electronic Arts Inc.'s ("EA") proprietary The Sims 4 commercial content catalog, comprising expansion packs, stuff packs, game packs, kits, and Marketplace items distributed through EA's PC platform, EA App. These works include both the content files themselves (game assets, textures, objects, and gameplay elements) and EA's proprietary internal entitlement database — a compilation of internal content identifiers (Item IDs and Entitlement Tags) used by EA's backend systems to verify user ownership. This entitlement database is an original, proprietary compilation protected under 17 U.S.C. § 106. The specific commercial titles affected span EA's entire The Sims 4 catalog, including but not limited to: Get to Work, City Living, Cats & Dogs, Seasons, Island Living, Snowy Escape, Cottage Living, High School Years, Growing Together, Horse Ranch, For Rent, Life & Death, Businesses & Hobbies, and all associated stuff packs, kits, and game packs released through the current date, as well as Marketplace content items sold exclusively through EA's in-platform virtual currency system.

**If the original work referenced above is available online, please provide a URL.**

https://www.ea.com/en/games/the-sims/the-sims-4

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/Leuansin/Leuans-sims4-toolkit

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

EA's The Sims 4 content — including expansion packs, stuff packs, kits, game packs, and Marketplace items — is protected by an access control measure in the form of EA's entitlement verification system. Before any user can access or install protected content, EA's backend systems authenticate the user's EA account and verify that the user holds a valid entitlement for that specific content, acquired either through purchase or through the authorized exchange of Sims Currency (EA's in-platform virtual currency for Marketplace content). Users who have not completed the required transaction are denied access; the content files are not delivered and the in-game content is not unlocked. This system effectively controls access to the works within the meaning of 17 U.S.C. § 1201, as access is conditioned entirely on successful authentication and entitlement verification through EA's controlled distribution infrastructure.

**How is the accused project designed to circumvent your technological protection measures?**

The repository distributes the source code for a toolkit that installs and configures "EA DLC Unlocker v2," a circumvention tool that injects a modified version.dll into The Sims 4 game installation directory. This DLL intercepts EA's entitlement verification calls at runtime and returns false positive responses, causing the EA App and Origin client to falsely report that the user owns DLC and Marketplace content they have not purchased, thereby bypassing EA's entitlement gate entirely.  
The repository additionally hosts and actively maintains a configuration file (Misc/g_s4_db.ini) containing EA's proprietary internal entitlement identifiers for 201 commercial content items. The toolkit's source code fetches this file at runtime, parses its entitlement tokens, and uses them to configure the circumvention tool to spoof ownership of specific EA content items. The toolkit further monitors this database for new entries and prompts users to update their circumvention configuration when new EA content becomes available — operating as an ongoing, actively maintained circumvention service. The circumvention is not incidental to the toolkit's function; it is the toolkit's explicit and primary purpose, as reflected in the tool's own user-facing language advising users they are "missing DLCs" and offering to configure the unlocker to include them.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/09u2h4n/Leuans-sims4-toolkit  
https://github.com/foxxelias/Leuans-sims4-toolkit  
https://github.com/jbf302/Leuans-sims4-toolkit  
https://github.com/mauricekleindienst/Leuans-sims4-toolkit

**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

The infringer goes by "[private]" on [private]. They use their [private] Server to point to the infringing content on GitHub: [private]

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

Electronic Arts Inc., [private]

**Please type your full name for your signature.**

[private]
