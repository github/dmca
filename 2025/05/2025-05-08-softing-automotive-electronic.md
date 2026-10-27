Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

Note: Because the parent repository was actively being forked when this DMCA takedown notice was received, and the submitter had identified all known forks at the time they submitted the takedown notice, GitHub processed the takedown notice against the entire network of 21 repositories, inclusive of the parent repository.

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

Yes.

**Please provide the Zendesk ticket number of your previously submitted notice. Zendesk ticket numbers are 7 digit ID numbers located in the subject line or body of your confirmation email.**

#3340924; previously, our requests dated April 14 and 16 were assigned the following Zendesk ticket numbers: #3345102 and #3346754 and #3340949

**Does your claim involve content on GitHub or npm.js?**

GitHub.

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

We represent Softing Automotive Electronic GmbH, [private] (hereinafter referred to as the “client”). As evidence of our authorization, we already submitted the power of attorney.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

It's about the “Softing Smart Diagnostic Engine” (SDE) software – Smart Diagnostic Engine; you can find information on the software at https://automotive.softing.com/products/softing-sde.html.

SDE is a platform-independent, high-performance runtime system designed for executing diagnostic functions, sequences, and services throughout the entire vehicle lifecycle. It supports standardized diagnostic formats like ODX and OTX, and offers a functional API that facilitates both simple and complex diagnostic tasks without requiring deep diagnostic expertise. SDE is versatile, supporting various applications including engineering, testing, manufacturing, after-sales, in-vehicle diagnostics, and cloud-based solutions, and is compatible with multiple operating systems such as Windows, Linux, Android, and iOS. The software layer SDE is used by many OEM customers ("Original Equipment Manufacturers"). OEM customers are companies that purchase products or components from another manufacturer for the purpose of integrating them into their own products or systems. These products are typically resold under the OEM’s own brand name. OEM customers do not use the products themselves but distribute them as part of a larger offering to end users.

To be able to talk to the vehicles, the software layer SDE requires a description for each control unit of how communication with this control unit works. In the case of SAE, this is the SMR data mentioned below.

These SMRs are created by SAE customers and contain customer secrets. That is why this data is also encrypted (see question 7). This data is distributed worldwide to all of the customers' workshops and production sites. With this protected information, it is possible, at least for older vehicles, to unlock and manipulate the control units.

This bypasses the protective measures taken by the OEMs. Newer vehicle generations contain better protective measures and cannot be manipulated so easily with this data. But even in this case, you can gain insights into how an OEM's control unit works and have better options for attacking it.

With the repository and forked repositories (see below) now available on GitHub, the encryption of our customers' SMR data can be removed. This means that anyone who has access to this SMR data can access our customers' secrets.

**If the original work referenced above is available online, please provide a URL.**

Smart Diagnostic Engine – Softing SDE | Softing, available at: https://automotive.softing.com/products/softing-sde.html

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing.

**Identify the full repository URL that is infringing:**

https://github.com/jglim/ODB

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes.

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

The technical protection measures used by our client to encrypt the data work as follows:

The relevant data is secured by a Blowfish encryption algorithm. The data is decrypted by an application that has a user licensing mechanism. The user does not see the data, but uses it to communicate with the control unit. The data is encrypted at all times and is only loaded into memory and used by the control unit's unlocking mechanism for a short period.

**How is the accused project designed to circumvent your technological protection measures?**

The software available at <Github.com> can be used to circumvent the technical protec-tion measure of our client’s software through encryption. We submit as Exhibit PBP 3 screenshots of the infringing repository. The software explicitly refers to our client's software.

The software describes in detail how it extracts the hidden private keys from the C++ libraries and uses them to remove the BLOWFISH encryption. The intention is clearly writ-ten to gain illegal access to the secrets hidden in the JAR files, which are contained in the encrypted data. The exact steps including the reverse engineering, the XOR transformation, the Blowfish decryption and inflation to circumvent our client’s protection mechanism is contained in the project description.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

I am reporting the following allegedly infringing forks:

https://github.com/653310641/ODB

https://github.com/anonbeat/ODB

https://github.com/anywhereyang/smr-d  

https://github.com/callumhutchy/ODB

https://github.com/codemasters1/ODB

https://github.com/dbstephan/ODB

https://github.com/digua0101/ODB

https://github.com/DiMO/ODB

https://github.com/drpsec1/ODB

https://github.com/kizdebski/ODB

https://github.com/kozik47/ODB

https://github.com/lndmnn/ODB

https://github.com/MasterCodeIT/ODB

https://github.com/mgeguren/ODB

https://github.com/pavmb/ODB

https://github.com/Pudel-des-Todes/ODB

https://github.com/raumlandbahnhof/ODB

https://github.com/topkekega/ODB

https://github.com/wgsoft-de/ODB

https://github.com/WSorban/ODB

**Is the work licensed under an open source license?**

No.

**What would be the best solution for the alleged infringement?**

Reported content must be removed.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

Our client does not have the contact details of the persons responsible for the software that enables such unauthorized circumvention of the copyright protection systems. However, according to the information on your company's website, the users with the GitHub names “[private]” and “[private]” seem to be responsible.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

YES.

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

YES.

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

YES.

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

YES.

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]

