While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

Yes

**Please provide the Zendesk ticket number of your previously submitted notice. Zendesk ticket numbers are 7 digit ID numbers located in the subject line or body of your confirmation email.**

3978127

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am the [private] and [private] creator of the source code

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The original work is [private] proprietary C# software 'CerberusWare' (hosted in [private] private repository). This is a modification for the game that adds new features to the game. By the date of creation of the repository and by its name, it is clear that [private] am the [private] of this project.

**If the original work referenced above is available online, please provide a URL.**

[private]

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/DobriyKaban/WebUi  
https://github.com/insvrg3ncy/autism-recode  
https://github.com/DobriyKaban/CerberusWareV3

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

The software is protected by a mandatory server-side authentication system (AuthSystem.cs) that verifies the user's Hardware ID (HWID) and a cryptographic key against [private] remote server.  
Additionally, the network communication is protected by SSL Certificate Pinning implemented in SecureHttpClient.cs, which validates the server's Subject Public Key Info (SPKI) hash to prevent Man-in-the-Middle attacks and unauthorized proxying.

**How is the accused project designed to circumvent your technological protection measures?**

The infringing repository contains a modified and “cracked” version of [private] software. The infringers used automated tools, likely AI/LLM-based as indicated by naming conventions, to refactor and rename classes and methods in order to make the proprietary code readable. They deliberately removed the AuthSystem logic and the AuthenticateAsync calls, thereby bypassing the mandatory license check. They also removed the SecureHttpClient SSL pinning validation to allow unauthorized traffic inspection and stripped out the Environment.Exit triggers intended to terminate the application when authentication fails. By removing these specific security mechanisms, the infringers have intentionally circumvented the access control measures [private] implemented to protect [private] copyright.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/superlowfpss/autism-decode  
https://github.com/superlowfpss/CerberusWareV3

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
