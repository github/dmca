While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf?**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

Me and [private] are developing a Minecraft modification. [private] is the copyright holder and has asked and permitted me to make a takedown request for this infringing repository.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed. If possible, include a URL to where it is posted online.**

The original work is a modification for the game Minecraft which adds several player assistance features. At this time it contains several ingame chat-commands and modules which allow the player to move faster or use similar quality-of-life improvements. It is proprietary and a license to use it is only given to people who pay for it or are selected for a free license.

There is a private [private] with its source code: [private]  
And there is a [private]which includes the price-list, announcements, and updated binaries of the installer: [private]

**What files should be taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**

https://github.com/PlutoSolutions/BaseBand  
https://github.com/PlutoSolutions/BaseBand/releases/download/1.0/baseband.zip

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

- Name obfuscation: Readable field, variable, and method names have been replaced with ambiguous names, e.g. Utils.writeString would become something like c.bg. This is done to make circumventing other security measures harder.
- String obfuscation: All text displayed to users or used internally is stored encrypted and only decrypted when used, in order to hide the operation of the software and make circumventing the other measures harder.
- JNI conversion (loader and installer only): Some java code is converted into zig code and compiled into dynamically linked libraties in order to make modification and disassembly of the software harder.
- Authentication and separation of "loader" and "core": To use the software, a user normally has to use a specialized installer which prompts them to log in. After installation, the login data is saved on their PC and the software's loader verifies it with a server. To use the software, the loader contacts the server, authenticates, and the server then sends the actual core of the software through an encrypted tunnel. This is then stored in RAM as described below:
- Data storage (the "DataKeeper"): Specialized data storage is used which checks the program and class accessing it, denying access when it is not allowed to. The core part of the software (the parts loaded by the loader) are never written to disk, and only kept in RAM in this special container.
- Anti-debugger: Many debuggers are detected by the loader and it will refuse to load the software if one is detected, because debuggers might be able to tamper with the authentication or intercept data.

**How is the accused project designed to circumvent your technological protection measures?**

The accused project is a "dump", that is an unauthorized copy of the mod's compiled core binary (.jar file). This was presumably done by letting the loader connect to a proxy and extracting the data from the connection it makes to the file server. In that case, the loader would connect to the proxy, authenticate, and the server (which the proxy connects to) then sends the (weakly) encrypted .jar file of the mod. [please redact here or remove these markers if that is not possible:] [private]. [redact end]

The end result is a full copy of the core of the modification, which was then slightly changed in order to make it work without the loader, which is normally not possible.

**<a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">Have you searched for any forks</a> of the allegedly infringing files or repositories? Each fork is a distinct repository and must be identified separately if you believe it is infringing and wish to have it taken down.**

None seem to be present at the time of writing.

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

[private] (preferred, both calling and SMS available)

[private]  
[private]  
[private]  
[private]  

**Please type your full legal name below to sign this request.**

[private]
