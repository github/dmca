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
  
I am a [private] of Multica.ai, the company that owns the copyright in the source code, documentation, and associated original creative material published at https://github.com/multica-ai/multica under the "multica-ai" GitHub organization. I am authorized to act on behalf of the company in matters of copyright enforcement, including the submission of this DMCA takedown notice.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
The Multica codebase is an original, open-source software project authored and published by our team at https://github.com/multica-ai/multica. We created the repository on 2026-01-13 and have been actively developing it in public ever since. The work consists of multiple original software components, including (non-exhaustively):  
  
- The daemon and agent runtime implementation (Go)  
- The Next.js + React web frontend and desktop application  
- The CLI tooling  
- The complete project documentation, README, and homepage copy  
- The LICENSE — a modified Apache License 2.0 with an additional commercial-use clause that we authored  
  
The entire contents of the repository at https://github.com/statica-ai/statica are infringing on our copyright and should be removed. statica-ai/statica is a verbatim copy of multica-ai/multica with the LICENSE file modified to substitute the original copyright holder with "© 2025 Statica, Inc." while preserving the rest of our license text (including the commercial-use clause we authored), with all references to Multica stripped from the README, and with the GitHub fork relationship intentionally severed by re-uploading the codebase as a standalone repository (`fork: false`) rather than using GitHub's fork mechanism. The repository's git history still contains commits authored by members of the Multica team.  
  
Specific evidence:  
1. statica-ai/statica was created 2026-04-17, three months after the multica-ai/multica public history begins (2026-01-13).  
2. The GitHub repository description on statica-ai/statica is the exact same one-line description we authored for multica-ai/multica: "The open-source managed agents platform. Turn coding agents into real teammates — assign tasks, track progress, compound skills."  
3. The LICENSE file in statica-ai/statica is structurally identical to ours, including the modified Apache 2.0 wording and the additional commercial-use clause we wrote, but with the copyright line replaced by "© 2025 Statica, Inc." — this is willful removal of copyright management information.  
4. The README contains no attribution to Multica or any acknowledgement of being a derivative work; it presents the project as original.  
5. The repository is published with `fork: false`, deliberately concealing its derivative relationship.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://github.com/multica-ai/multica  
https://multica.ai  
https://github.com/multica-ai/multica/blob/main/LICENSE  
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
The entire repository is infringing  
  
**Identify the full repository URL that is infringing:**  
  
https://github.com/statica-ai/statica  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
https://github.com/statica-ai/statica  
  
**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**  
  
**Is the work licensed under an open source license?**  
  
Yes  
  
**Which license?**  
  
apache-license-2.0  
  
**How do you believe the license is being violated?**  
  
Multica is published under a modified Apache License 2.0. Both the unmodified Apache 2.0 and our modified version require, at minimum:  
  
1. Preservation of all existing copyright, patent, trademark, and attribution notices from the source — including a NOTICE file where present.  
2. Inclusion of a copy of the License with any redistribution.  
3. Clearly indicating any modifications made to derivative work.  
  
statica-ai/statica violates all three:  
  
1. The LICENSE file in statica-ai/statica has been edited to remove the original copyright holder and substitute "© 2025 Statica, Inc." in its place, while preserving the rest of our license text (including the additional commercial-use clause we authored). This is a direct removal of copyright management information.  
2. The README contains no attribution to multica-ai/multica, no reference to the upstream project, and no acknowledgment of being a derivative work. The repository presents itself as an original project.  
3. The repository was uploaded as a standalone (`fork: false`) GitHub repository rather than as a GitHub fork, deliberately severing the traceability that the platform provides for derivative works.  
  
Additionally, our modified Apache 2.0 license imposes a commercial-use restriction (which they copied verbatim into their LICENSE). statica-ai/statica is operating a commercial-looking website at [private], which appears to violate that clause as well — although the primary violation we are reporting here is the removal of the original copyright notice and attribution.  
  
**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**  
  
To bring the repository into compliance with our modified Apache 2.0 license, the operators of statica-ai/statica would need to do all of the following:  
  
1. Restore the original LICENSE file from multica-ai/multica verbatim, including the original copyright holder line. They may add their own copyright on top of the original for portions they have themselves contributed, but the original copyright holder must remain.  
2. Add a clear, prominent attribution at the top of the README that (a) identifies multica-ai/multica as the upstream project, (b) links to https://github.com/multica-ai/multica, and (c) states that statica-ai/statica is a derivative work.  
3. Clearly indicate, either in the README or in a CHANGES / NOTICE file, what modifications they have made on top of the upstream project, in accordance with section 4(b) of Apache License 2.0.  
4. Either re-publish the repository as a GitHub fork of multica-ai/multica (using GitHub's fork mechanism to preserve traceability), or — if they prefer to keep it as a standalone repo — explicitly state the derivative relationship in the repository description and README.  
5. If they intend to use the project commercially (e.g., for the service at [private]), obtain a commercial license from us, consistent with the commercial-use clause they copied from our LICENSE.  
  
Absent the above remediation, we request removal of the entire repository.  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
We do not have direct contact information for the operators of statica-ai/statica. The following public-facing channels associated with the project may be useful:  
  
- Website / homepage: [private]  
- [private]  
- GitHub organization: https://github.com/statica-ai  
  
We have not contacted them privately prior to this notice.  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]  
