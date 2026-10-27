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

I am [private], [private] of Blue Wizard Digital Inc. (“BWD”), the developer and publisher of the videogame Shell Shockers.  
BWD is the sole owner of all copyrights in the Shell Shockers game client, artwork, audio, and related code. I am fully authorized to act on BWD’s behalf in enforcing those rights.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The copyrighted work is the complete client‑side JavaScript codebase of *Shell Shockers* (first released 2017, © 2017‑2025 Blue Wizard Digital Ltd.).
The code contains proprietary gameplay logic, networking protocols, item and weapon statistics, map parsers, asset references, authentication routines, and anti‑cheat measures.  
Blue Wizard Digital distributes the client solely through https://shellshock.io in a minified/obfuscated form and has **never released or licensed the unobfuscated source code to the public**.

**If the original work referenced above is available online, please provide a URL.**

https://shellshock.io

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/yolkorg/yolkbot

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

The *Shell Shockers* client is deliberately obfuscated and authenticated through signed WebSocket handshakes that validate user sessions and device fingerprints.
Server‑side anti‑cheat routines monitor packet rate, sequence, and integrity to block modified or automated clients.  
These combined measures prevent third parties from obtaining unobfuscated source code or connecting to live game servers without the official client.

**How is the accused project designed to circumvent your technological protection measures?**

The project embeds our Firebase API key, hard‑coded packet enums, and a script ([private]) that forges the authenticated WebSocket handshake, allowing an automated script to connect directly to wss://shellshock.io/matchmaker/ without launching the authorised client or passing anti‑cheat checks.  
Bot APIs (e.g., Bot.fire(), Bot.moveTo()) generate player‑update, fire, and melee packets at machine speed, defeating server rate limits and gameplay integrity checks.  
Scraper scripts strip obfuscation by harvesting maps, item stats, and language files from the live CDN, then republish them inside the repo.  
Collectively these features are “technologies … primarily designed or produced for the purpose of circumventing” our access‑control and anti‑cheat measures, in violation of 17 U.S.C. §1201.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

I do not, they remain anonymous for obvious reasons.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
