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

I am the [private] and copyright owner of the Tradecopia desktop trading application (a proprietary Go + Svelte application distributed only as a compiled binary). All source code, compiled binaries, and bundled frontend assets are [private] original, unpublished, proprietary work, distributed under a proprietary license that prohibits reverse engineering and redistribution.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The Tradecopia desktop application — proprietary closed-source software comprising Go backend source code (internal API, authentication, credential storage, replication manager, risk manager, broker platform integrations) and a compiled Svelte/JavaScript frontend. The application is distributed to end users solely as a compiled, licensed binary. The reported repository contains: (a) Ghidra disassembly of [private] compiled functions, (b) [private] complete internal function/symbol index and original source file paths, (c) [private] full frontend JavaScript bundles copied verbatim from the shipped application, and (d) extracted internal string/schema constants — all derived directly from [private] proprietary application without authorization.

**If the original work referenced above is available online, please provide a URL.**

https://github.com/tradecopia/tradecopia-desktop

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/stolencompute/tradecopia-reverse-engineered

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

The application is distributed only in compiled form and its functionality is gated by server-side authentication and subscription licensing. The reported repository decompiles the protected binary and implements a mock/proxy server specifically to circumvent that access-control and licensing mechanism.

**How is the accused project designed to circumvent your technological protection measures?**

The Tradecopia desktop application is distributed only as a compiled binary, and its functionality is gated by server-side authentication and subscription licensing that clients access over HTTPS. The accused project is designed to defeat both measures. It (1) decompiles the protected compiled binary using Ghidra and republishes the resulting disassembly, internal function/symbol index, source-file paths, and extracted string constants; (2) republishes the application's compiled frontend JavaScript bundles verbatim; and (3) implements a mitmproxy capture script and a local "mock/proxy server" (tradecopia_mitm_capture.py, tradecopia_mock_server.py) whose stated purpose, per the repository's own README and issue #1, is to "bypass Tradecopia's cloud dependency" — intercepting the licensing/auth traffic, caching the authentication token, forging mock authentication responses, and stubbing the remaining server endpoints so the licensed application runs decoupled from the server-side controls that enforce authentication and subscription entitlement.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

GitHub username: stolencompute (https://github.com/stolencompute), display name "[private]." No email, telephone, or physical address is known to me.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
