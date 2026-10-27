Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
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

I am a [private] who studies research papers in AI/ML nature.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed. If possible, include a URL to where it is posted online.**

Copyright owner's repo: https://github.com/OrigamiDream/gato  
Infringer's repo: https://github.com/kyegomez/GATO

I've explicitly mentioned that the copyright owner's repo is following MIT license, which has condition for license and copyright notice.
The infringer has copied README and codes, which are important part in copyright owner's repo.

The infringer didn't mention about the license which is belonging to copyright owner's repo, even I had asked for them, he ignored and closed the issue.

https://github.com/kyegomez/GATO/blob/master/README.md#dataset-and-model-architecture

https://github.com/kyegomez/GATO/blob/master/gato/model.py#L45

https://github.com/kyegomez/GATO/blob/master/gato/model.py#L422

`README.md`:  
- The infringer copied images that explains how the repo works and analysis of research paper.  
- The infringer copied my paper reviews including Architecture Variants, Residual Embedding, and Position Encodings, etc.  
- Especially, the infringer had uploaded my README file with minimum modification at first (the first commit, [65604ca](https://github.com/kyegomez/GATO/blob/65604ca189e0a6e2e79db554403b40e35ab0819c/README.md)) and the infringer modified that silently over time.

`gato/model.py`:  
- The infringer had uploaded codes with only converting base framework (TensorFlow -> PyTorch) and without changing my directory structure, etc at first (the first commit, [65604ca](https://github.com/kyegomez/GATO/tree/65604ca189e0a6e2e79db554403b40e35ab0819c/gato)) and the infringer modified them silently over time.
- The latest commit in master branch has different directory structure as a result of the consistent modification, but the components in `gato/model.py` still does not have originality, especially [PatchPositionEncoding](https://github.com/kyegomez/GATO/blob/master/gato/model.py#L45) and `forward(...)` function in [Gato]  
(https://github.com/kyegomez/GATO/blob/master/gato/model.py#L422) are almost same with [private] original codes: [PatchPositionEncoding]  (https://github.com/OrigamiDream/gato/blob/main/gato/models/embedding.py#L19), [Gato](https://github.com/OrigamiDream/gato/blob/main/gato/models/__init__.py#L25)

**What files should be taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**

I believe that the entire repository is enough to be taken down since the infringed parts (README and gato/model) are the most important part of copyright owner's repo.

The list of the repositories:  
https://github.com/kyegomez/GATO

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

No

**<a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">Have you searched for any forks</a> of the allegedly infringing files or repositories? Each fork is a distinct repository and must be identified separately if you believe it is infringing and wish to have it taken down.**

I have searched for the forks, but there seems no infringements.

**Is the work licensed under an open source license?**

Yes

**Which license?**

MIT

**How do you believe the license is being violated?**

The MIT license requires copyright notice in their repo if they utilizes codes under the license.

Excerpted part of the license terms:  
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.

But the infringer didn't mention about the license under the copyright owner's repo, and ignored asks of copyright owner.

**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**

The infringer can add copyright notice at the top of README in their repo.

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

[private] (prior [private]): [private]  
Email: [private]

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

Phone number: [private]  
Physical address:  
- Country: [private]  
- State/Region: [private]  
- City: [private]  
- Address: [private]

**Please type your full legal name below to sign this request.**

[private]
