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

I am [private] for Electronic Arts Inc. ("EA") and am authorized to act on EA's behalf. EA is the copyright owner of The Sims 4 video game and all DLC.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The infringed works are Electronic Arts Inc.'s ("EA") proprietary The Sims 4 video game catalog, comprising expansion packs, stuff packs, game packs, kits, and Marketplace items distributed through the EA App and Steam. These works include the content files themselves (game assets, textures, objects, and gameplay elements) and EA's proprietary internal entitlement database — a compilation of internal content identifiers used by EA's backend systems to verify user ownership. The specific commercial titles affected span EA's entire The Sims 4 catalog from launch through the current date, including all expansion packs (EP01–EP20+), game packs, stuff packs, kits, and Marketplace content items sold exclusively through EA's in-platform virtual currency system.

**If the original work referenced above is available online, please provide a URL.**

https://www.ea.com/en/games/the-sims/the-sims-4/buy

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

[private]

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

The works are protected by an access control measure: EA's entitlement verification system. Before any user can access or install protected content, EA's backend systems authenticate the user's EA account and verify that the user holds a valid entitlement for that specific content, acquired either through purchase or through the authorized exchange of Sims Currency for Marketplace content. Users who have not completed the required transaction are denied access. This system effectively controls access to the works within the meaning of 17 U.S.C. § 1201(a)(3)(B), as access is conditioned entirely on successful authentication and entitlement verification through EA's controlled distribution infrastructure.

**How is the accused project designed to circumvent your technological protection measures?**

The repository distributes source code for a toolkit containing a dedicated "DLC Unlocker" component that installs a modified version.dll into the Sims 4 game installation directory. This DLL intercepts EA's entitlement verification calls at runtime and returns false positive responses, causing the EA App and Origin client to falsely report that the user owns DLC and Marketplace content they have not purchased, thereby bypassing EA's entitlement gate entirely.
The repository additionally hosts and actively maintains Misc/g_s4_db.ini, a configuration file containing EA's proprietary internal entitlement identifiers for 201 commercial content items. The toolkit's source code (DLCUnlockerWindow.xaml.cs) fetches this file at runtime via authenticated HTTP requests to GitHub's raw content CDN, parses its entitlement tokens, and uses them to configure the circumvention tool to spoof ownership of specific EA content. The toolkit further monitors this file for updates using HTTP ETag caching and prompts users to update their circumvention configuration when new EA content becomes available — operating as an ongoing, actively maintained circumvention service. The tool's own user-facing messaging makes the purpose explicit: it notifies users they are "missing DLCs" and offers to configure the unlocker to include them. The circumvention is the explicit and primary purpose of the repository.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

[private]  
[private]  
https://github.com/dawsonjames612-source/Leuans-sims4-toolkit  
https://github.com/DexyThePuppy/Leuans-sims4-toolkit  
[private]  
[private]  
[private]  
https://github.com/nikschwarz11-blip/Leuans-sims4-toolkit  
https://github.com/Nue1-0/Leuans-sims4-toolkit  
https://github.com/Paddychief92/Leuans-sims4-toolkit  
https://github.com/Sxxjava/Leuans-sims4-toolkit  

**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**

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
