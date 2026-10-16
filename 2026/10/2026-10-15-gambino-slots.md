Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am the [private] and authorized legal representative of Spiral Interactive (the owner and developer of Gambino Slots). Spiral Interactive is the sole and exclusive owner of all copyrights, trademarks, software code, visual artwork, audio files, and proprietary game assets associated with the "Gambino Slots" game. In my [private] [private] capacity, I am fully authorized by Spiral Interactive to act on its behalf in all matters concerning intellectual property protection, enforcement, and the submission of DMCA takedown notices.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The copyrighted works comprise the proprietary audiovisual assets, software, and creative materials embodied in the HTML5 web and mobile game "Gambino Slots" (accessible via https://www.gambinoslot.com/ and official app distribution platforms), developed, owned, and operated exclusively by Spiral Interactive.

These original, protected works include:  
1. Visual and Graphic Artwork: Proprietary 2D and 3D graphic illustrations, slot reel symbol artwork, character renderings, theme backgrounds, UI/UX icon sets, textures, sprite atlases, and custom typography created specifically for Gambino Slots.  
2. Motion & Animation Assets: Proprietary skeletal animation files (e.g., Spine JSON rigs, atlases, and animation sequencing data) designed for in-game mechanics and visual effects.  
3. Audio Works: Original audio recordings, sound effects, musical arrangements, win celebrations, and voice-over tracks produced and licensed exclusively for the Gambino Slots title.  
4. Client-Side Software Architecture: Proprietary client asset manifests, structured configuration files, and game asset bundles compiled specifically to power the Gambino Slots H5 client.

All such creative assets and compiled client data are original works of authorship protected under international and United States copyright laws, with all exclusive rights reserved by Spiral Interactive.

**If the original work referenced above is available online, please provide a URL.**

https://www.gambinoslot.com/  
[private]  
[private]  
[private]  

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/14degrees/gambino-explorer

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Spiral Interactive protects its copyrighted game assets, client code, and audiovisual content through multiple layers of technological protection measures under 17 U.S.C. § 1201:

1. Dynamic Session Authentication & Protocol Integrity: Access to our production servers and asset endpoints requires valid, cryptographically generated session tokens and application state handshakes. These controls ensure that game data and asset bundles are transmitted exclusively to legitimate, authorized runtime instances of our H5 client during active gameplay sessions.

2. Asset Packaging and Obfuscation: Our proprietary 2D/3D artwork, spine animations, sound banks, and configuration schemas are not delivered as open, browsable directories. They are compiled into proprietary asset manifests, serialized bundles, and obfuscated formats designed to execute solely within the compiled client engine and prevent direct scraping or export.

3. Execution Integrity Controls: The client-side logic is heavily minified, obfuscated, and bound to proprietary delivery workflows to prevent unauthorized modification, inspection, or extraction of underlying raw creative works.

How These Measures Control Access:  
In the ordinary course of operation, these controls ensure that users can only view and experience the copyrighted works ephemerally within the authorized Gambino Slots client interface. Raw source files (such as master textures, audio stems, spine rigs, and schema payloads) are inaccessible to normal end-users.

The repository owner circumvented these effective controls by analyzing runtime memory, intercepting session-authenticated network traffic, reversing our asset-packing mechanisms, and authoring custom extraction scripts to bypass our intended presentation layer and rip the raw copyrighted assets directly to disk.

**How is the accused project designed to circumvent your technological protection measures?**

The accused project (gambino-explorer) is specifically designed and engineered to circumvent our technological protection measures in the following ways:

1. Reverse-Engineering Delivery Pipelines: The author reverse-engineered the Gambino Slots H5 client code, mapping out the proprietary asset-bundling schemes, naming structures, and internal asset-manifest structures that are intentionally obfuscated to prevent external indexing and scraping.

2. Automated Extraction Tooling: The repository contains purpose-built scripts and tooling designed to parse our proprietary manifest files, unpack asset bundles, reconstruct skeletal spine rigs, and extract raw, master-quality image assets (textures/sprites) and audio stems directly to disk, bypassing our client's rendering pipeline.

3. Interception and Replay of Authenticated API Traffic: The project captures, documents, and provides tools to simulate/replay authenticated production network sessions and API calls outside of an authorized gameplay session, effectively circumventing the session-based controls that govern client-server asset delivery.

4. Public Distribution of Ripped Assets: Beyond the tools themselves, the author used these circumvention mechanisms to mass-rip our copyrighted materials from our production environment and host static copies directly within the GitHub repository for unauthorized public access.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

We do not possess direct personal contact details (such as a personal email address, phone number, or physical address) for the repository owner.

Their only known public identifiers are their GitHub username and public profile:  
GitHub Username: 14degrees  
Profile URL: https://github.com/14degrees  
Repository URL: https://github.com/14degrees/gambino-explorer

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
