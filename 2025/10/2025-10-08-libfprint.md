**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

Yes

**Please provide the Zendesk ticket number of your previously submitted notice. Zendesk ticket numbers are 7 digit ID numbers located in the subject line or body of your confirmation email.**

cc6f582

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I'm one of the [private] of libfprint (principally hosted at https://gitlab.freedesktop.org/libfprint/libfprint, [private] also have a fork at https://github.com/3v1n0/libfprint) and lots of code there has been written or contributed by [private] and [private] and it's all under the LGPL copyright.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The libfprint (all the core, minus some drivers code) code (LGPL) has been compiled together with some non-LGPL-compatible code that has been written by [private].

This is easily visible just using `string` on them, or more closely by using ghidra or other reverse engineer software, it's clearly visible that the libraries contain libfprint code together with closed source code they included.

They also are meant to be a replacement for system libfprint libraries, which is also a clear reason for it.

They are now sharing through github these pre-compiled libraries that contain both LGPL code and proprietary code.

**If the original work referenced above is available online, please provide a URL.**

https://gitlab.freedesktop.org/libfprint/libfprint

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/ftfpteams/focaltech-linux-fingerprint-driver  
https://github.com/ftfpteams/ubuntu_spi  
https://github.com/ftfpteams/RTS5811-FT9366-fingerprint-linux-driver-with-VID-2808-and-PID-a658  

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

No

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/solarkennedy/ubuntu_spi  
https://github.com/MasterK0927/Fingerprint-Setup-For-a658-fp

**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**

**Is the work licensed under an open source license?**

Yes

**Which license?**

gnu-lesser-general-public-license-v2.1

**How do you believe the license is being violated?**

The binaries included are the full libfprint code, compiled with additional proprietary drivers code and none of such code has been released.

They disclosed some of the loader code https://gitlab.freedesktop.org/libfprint/libfprint/-/merge_requests/396 but not the full driver source code.

**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**

Releasing their driver source code or dynamic loading the proprietary source code, although this is controversial.

But there's a libfprint-TOD fork that could have been used safely for this purpose.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

We have not direct contacts, we only tried to contact them via [private] and the project github issue

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
