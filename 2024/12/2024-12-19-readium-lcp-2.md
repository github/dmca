Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf?**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

Readium LCP is a DRM solution, i.e. a set of technological measures of copyright protection through copy control and encryption. Ebook distributors, e.g. public libraries and online booksellers, are using LCP to limit the dates between which a publication can be read, the number of characters a user is able to copy/paste or the number of pages a user may print. They can also control that the content is not overshared.

The core LCP specification (https://readium.org/lcp-specs/releases/lcp/latest) defines the notion of encryption profiles. Encryption profiles are a way to include a secret algorithm in an otherwise open solution, secrets being the basis of effective technical measures of protection.

We are the copyright owner of the official LCP encryption profile used by ebook distributors worldwide (named Profile 1.0). More information about our role relative to LCP is found in https://www.edrlab.org/readium-lcp/. LCP adopters are listed in https://www.edrlab.org/readium-lcp/certified-apps-servers/.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed. If possible, include a URL to where it is posted online.**

The LCP encryption profile 1.0, its algorithm and associated keys are not posted online, nor open in any manner. Instead they must be licensed through an explicit agreement with EDRLab.

**What files should be taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**

The following git repository contains circumvention technology that enables users to illegally access ebooks protected by copyright:

https://github.com/OothecaPickle/DeDRM_tools

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

The protection is based on the encryption of each resource of a publication. Without going into details of the LCP technological measures, only trusted LCP compliant client applications can give access to unencrypted content. In order to decrypt such encrypted content, LCP compliant applications use a user passphrase and a secret algorithm which is part of the LCP encryption Profile 1.0. LCP compliant client applications cannot save decrypted ebooks.

LCP possesses several interesting characteristics:

- This DRM solution is available on every client platform (iOS, Android, Windows, MacOS, Linux, specific eInk Readers); with the open-source Readium development toolkits, EDRLab provides an open-source codebase which eases such integration. LCP does not lock the user into a proprietary environment. This is why e-lending libraries are moving to this solution, worldwide.

- The DRM solution is especially useful for library e-lending, as it provides loan extensions and early return features.

- This DRM solution does not hurt the accessibility of the content it protects, for people with reading impairment; this is why is it promoted by the DAISY Consortium.

- This DRM solution lets users share LCP protected ebooks with friends and family; there is no predefined number of devices which can use an LCP protected ebook.

- This is the only DRM solution which is based on an open standard. This means that inspecting the software for flaws is easy. The encryption profile is the only confidential information.

**How is the accused project designed to circumvent your technological protection measures?**

The user has published on GitHub software which specifically allows the decryption of ebooks protected by the LCP Profile 1.0 and allows saving them as non-protected ebooks. This infringement violates our legal business and affects authors and publishers’ IP.

This codebase is presented as a plug-in of the well-known Calibre software, an open-source ebook manager.

The file which explicitly deals with LCP circumvention is:  
https://github.com/OothecaPickle/DeDRM_tools/blob/4d4368598f5d1ada0c5e8f6406205e006f258d65/DeDRM_plugin/lcpdedrm.py

It contains a "master key" which should not be exposed, and corresponds to a hack of the algorithm of the LCP Profile 1.0.

**<a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">Have you searched for any forks</a> of the allegedly infringing files or repositories? Each fork is a distinct repository and must be identified separately if you believe it is infringing and wish to have it taken down.**

There is currently no fork of the infringing project.

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Other Change

**Describe the change.**

The purpose of this repository is to illegally access copyrighted material, and the content of the repo was illegally obtained. The repo and its forks must be shut down entirely.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

We have no information regarding the identity of the alleged infringer beyond his Github account name.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]  
[private]  

**Please type your full legal name below to sign this request.**

[private]  
