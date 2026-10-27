While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am the copyright holder and [private] of Adaptive Machines, Inc. (d/b/a Runner AI), a [private]
corporation with its principal place of business at [private].
Adaptive Machines, Inc. is the sole owner of all copyrights in the Runner AI platform, including the
scaffold storefront source code, build scripts, Runner AI skill definitions, and configuration files
contained in the infringing repository. This code was developed internally by our engineering team, has
never been released under any open source license, and is confidential proprietary software as defined in
our Terms of Service (Section 6 - Confidentiality). Our ToS (Section 1) explicitly prohibits users from
reproducing, duplicating, copying, or exploiting any portion of the Service without express written
permission. Additionally, the repository exposes our private Google Cloud service account credentials and
private npm registry configuration, which are trade secrets.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

▎ The copyrighted works are:  
▎ 1. Runner AI Scaffold Storefront — A proprietary Next.js/Vite storefront template deployed into [private]
sandboxes for Runner AI users. This includes all source code under the storefront/ directory: React
components, routing logic, checkout flows, product display components, booking system, payment integration,
and build configuration.  
▎ 2. Runner AI Skill Definitions — Proprietary AI agent skill files located at storefront/.runner/skills/,
including building-storefronts/SKILL.md and implementing-product-reviews/SKILL.md with associated reference
documentation. These define Runner AI's proprietary AI coding assistant behavior.  
▎ 3. Runner AI Context/Checkpoint System — Proprietary conversation context and state management files
under runner/context/, including checkpoint data (messages.json, recovery-hints.json, state.md).  
▎ 4. Build and Deployment Scripts — Proprietary shell scripts (build-and-store.sh, commit-and-cache.sh,
rollback-with-cache.sh, serve-build.sh) that are part of Runner AI's infrastructure.  
▎ 5. Exposed Credentials — The repository also contains our private Google Cloud service account key
(storefront/npm_registry_service_account.json) and private npm registry configuration (storefront/.npmrc)
pointing to our private Google Artifact Registry, which are confidential and proprietary.

▎ None of this code has ever been authorized for public distribution.

**If the original work referenced above is available online, please provide a URL.**

▎ The original work is proprietary and not publicly available. The authorized production platform is at
https://runnerai.com. The scaffold code is deployed only into private sandbox environments for
authorized users. Documentation is at https://docs.runnerai.com.

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/aarizpe95/nucleopep-runnerai

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Yes. The scaffold source code is deployed only into private, ephemeral [private] sandbox environments with
authenticated access. The GCP service account key and private npm registry are protected by Google Cloud
IAM access controls. The source code is maintained in private repositories. Users are bound by our Terms of
Service which prohibit reproduction, duplication, copying, or reverse engineering of the Service (ToS
Section 1)

**How is the accused project designed to circumvent your technological protection measures?**

▎ The Runner AI platform deploys proprietary scaffold source code exclusively into private, ephemeral
[private] sandbox environments that are authenticated and access-controlled — users interact with the code
only through Runner AI's web interface (iframe-based preview and AI-assisted editing). The code is never
intended to be extracted, downloaded in bulk, or published outside the sandbox.

▎ The accused party extracted the entire contents of a private [private] sandbox — including files that serve
as technological access controls — and published them to a public GitHub repository. Specifically:

▎ 1. Private npm registry credentials extracted and published: The file
storefront/npm_registry_service_account.json is a Google Cloud service account private key that
authenticates access to our private Google Artifact Registry
(us-west2-npm.pkg.dev/stable-course-466811-k1/medusa-fork/). This key was provisioned exclusively for use
within the sandbox runtime. By extracting and publishing it, the accused has exposed the authentication
mechanism that controls access to our proprietary packages, allowing anyone to download our private
@[private] fork.  
▎ 2. Registry authentication configuration extracted: The file storefront/.npmrc contains the private
registry URL, and the preinstall script in package.json
([private]) reveals
the exact authentication flow. Together, these files enable any third party to bypass our access controls
and pull packages from our private registry.  
▎ 3. Full project export archive included: The repository contains storefront/prj_export.tar.gz (~3.9 MB),
which appears to be a bulk export of the sandbox project contents, circumventing the intended file-by-file
access model of the sandbox environment.

▎ In summary, the accused extracted proprietary code from a controlled sandbox environment and published
both the code and the credentials that protect access to our private package infrastructure, effectively
circumventing all technological measures in place.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

[private]

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
