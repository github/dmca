While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

One or more repositories in this DMCA takedown notice has been processed in accordance with GitHub's prohibition on sharing unauthorized product licensing keys, software for generating unauthorized product licensing keys, and/or software for bypassing checks for product licensing keys.

You can learn more in [GitHub's Acceptable Use Policies](https://docs.github.com/en/github/site-policy/github-acceptable-use-policies).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am the [private] and copyright owner of Xenon Security API (https://xenonsecurity.net, https://panel.xenonsecurity.net/), a proprietary software licensing API and delivery platform. [private] develop and distribute this software through [private] [private] company.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The copyrighted works include: (1) the Xenon loader binary ("loader.exe"), the client-side component of [private] licensing platform; (2) the protected DLL artifact delivered exclusively to authorized licensees through the Xenon API; and (3) the Xenon authentication and delivery API protocol. These are original works of authorship, distributed exclusively under a proprietary license through https://xenonsecurity.net.

**If the original work referenced above is available online, please provide a URL.**

https://xenonsecurity.net

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/maisou/xdnon-dep

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Yes. [private] software employs multiple technological protection measures: (1) TLS certificate validation ensuring the loader communicates only with [private] authentic server; (2) DNS-based server identity verification; (3) a multi-endpoint authentication protocol requiring valid license keys, session IDs, hardware IDs, and owner IDs across endpoints including /api/machine/initialize, /api/machine/watchdog, /api/machine/diagnostics, and /api/license/log; (4) authenticated artifact delivery through /api/artifact//download only after successful authentication.*

**How is the accused project designed to circumvent your technological protection measures?**

The infringing tool (xdnon.exe) systematically defeats all of these protections through a 7-phase attack: (1) generates and installs a self-signed root CA certificate for xenonsecurity.net; (2) hijacks DNS by modifying the Windows hosts file to redirect [private]  domain to localhost; (3) runs a local HTTPS server on port 443 with the fake certificate; (4) intercepts all [private]  API endpoints and returns forged success responses; (5) constructs and submits forged authentication queries; (6) launches [private]  stolen loader which connects to the fake server, passes all forged auth, and loads the stolen DLL; (7) removes the fake certificate and hosts file entries to destroy evidence. The tool additionally randomizes PE metadata on served binaries to evade hash-based detection. The tool has no commercially significant purpose other than circumventing [private] access controls.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

The GitHub account is "maisou" (https://github.com/maisou). I do not have additional contact information.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
