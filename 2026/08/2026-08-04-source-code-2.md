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

I am the copyright owner of the original work described below.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

Repository: [private]

File path: src/broadcast_relay.c

Commit OID: 1e412cb53fb566f5084d889433de4ff207162d0c  
Author / copyright notice in file header: “Copyright (C) 2025 [private]”

License: GNU General Public License (GPL), version 2 or later (stated in the file header)

Purpose / high-level description: a single-source C implementation of an Access+/ShareFS “broadcast relay” used for NAT mode networking. It bridges Access+ UDP broadcasts and SLiRP/NATed guest networking: creating/binding sockets on specific Access+ ports, relaying outgoing guest Access+ datagrams to the host network (including broadcast handling), and repackaging/injecting incoming host responses into the guest’s virtual network. It also contains logic for IP fragmentation and (critically) reassembly of guest-originated fragmented UDP datagrams so large ShareFS transfers complete correctly.

**If the original work referenced above is available online, please provide a URL.**

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

https://github.com/macpato82/KineticBox2/blob/baseline/src/RelayNetwork.c

https://github.com/macpato82/KineticBox2/blob/windows/src/RelayNetwork.c

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

No

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

Yes

**Which license?**

gnu-general-public-license-v3.0

**How do you believe the license is being violated?**

The original file ([private], 2025 [private]) is copyrighted and released under the GNU GPL v2 (or later) as stated in its file header.

The file in the other repository (macpato82/KineticBox2/blob/windows/src/RelayNetwork.c) contains code that is substantially identical in structure, function names, comments, constants, and implementation logic.

Many blocks are effectively verbatim or trivially edited (e.g., small identifier replacements such as logging function name), indicating direct copying rather than independent re-implementation.

When code is copied from a GPL-licensed project, the GPL requires that derivative works preserve the original copyright notices, license terms, and attribution in the source files. If a project includes GPLed code, it must comply with the GPL’s obligations: include the same GPL license, preserve the original copyright notice(s), and distribute under GPL terms.

Specific ways the license appears to be violated...

Removal / alteration of original copyright/attribution:

The copied file in macpato82/KineticBox2 (RelayNetwork.c) has a different header claiming “Copyright (C) 2026 [private]” and does not include the original “Copyright (C) 2025 [private]” notice from the rpcemu-extended file. Replacing or omitting the original author and year while retaining otherwise-identical code is a failure to preserve required copyright and attribution notices.

No preserved license boilerplate or attribution:

Although the KineticBox2 file includes a GPL header, it does not retain the original author attribution ([private]) and file-specific header content from the rpcemu-extended file. The GPL requires that copyright and license notices be preserved when reusing GPLed source.

Evidence of direct copy with minor token changes:

Large contiguous code blocks (comments, constant values, function names, and control flow) match the original. In many places the only changes are renaming of logging calls or project-specific include names. This pattern is consistent with copy/paste and superficial find/replace edits rather than an independent re-implementation. Under the GPL, that is a derivative work and must preserve the original notices and license text and attribute the original author.

Potential misrepresentation of authorship:

By changing the file header to claim a different copyright holder without preserving the original attribution, the repository is effectively presenting the code as authored by the new copyright owner.

That misrepresents the original author’s contribution and violates the obligation to preserve copyright notices. Even if the new repository distributes under GPL as well, removal of the original copyright line is not permitted.

If the code were intended to be original, it is not - the substantial similarity and matching unique design choices (reassembly algorithm, exact constants, comments) contradict that. Because the original is GPL, copying it creates a GPL derivative that must follow the GPL conditions exactly (including attribution).

**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**

Restore the original copyright/author and GPL header from [private] in the macpato82 file (preserve the original copyright line(s), year, and license statement), and retain a note of any modifications (typical approach: add “Modified by …” lines).

Alternatively, remove the copied code from macpato82/KineticBox2 or replace it with a genuinely original implementation that does not derive from the rpcemu-extended source.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
