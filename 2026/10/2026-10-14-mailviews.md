**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**  
  
Yes, I am the copyright holder.  
  
**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**  
  
Yes  
  
**Please provide the Zendesk ticket number of your previously submitted notice. Zendesk ticket numbers are 7 digit ID numbers located in the subject line or body of your confirmation email.**  
  
4767035  
  
**Does your claim involve content on GitHub or npm.js?**  
  
GitHub  
  
**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**  
  
I am the [private] of the Mailviews email component library, and I am authorized to act on behalf of its copyright owner, Maizzle OÜ ([private]). Maizzle OÜ ([private] company) publishes and sells the library at https://mailviews.com.  
  
[private] wrote the component markup and styling, and the demo text shown in each component. [private] also named and produced the image assets that ship with the components. Mailviews has been sold under a paid, non-open-source license since November 19, 2025.  
  
This is a revised notice. GitHub [private] asked [private] to clarify whether I claim the entire repository. I do: the entire repository https://github.com/shadcn-labs/emailcn and its forks.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
SCOPE OF THIS CLAIM: THE ENTIRE REPOSITORY  
  
I claim that the ENTIRE repository https://github.com/shadcn-labs/emailcn is infringing, including all of its branches:  
- main  
- codex/email-studio (https://github.com/shadcn-labs/emailcn/tree/codex/email-studio), which contains the same copied files at the same paths.  
  
The repository exists to redistribute [private] commercial Mailviews component library for free under an MIT license, ported to React Email, MJML React and JSX Email.  
  
The copied library is the core of the repository:  
- https://github.com/shadcn-labs/emailcn/tree/main/registry/bases/react-email/components  
- https://github.com/shadcn-labs/emailcn/tree/main/registry/bases/mjml-react/components  
- https://github.com/shadcn-labs/emailcn/tree/main/registry/bases/jsx-email/components  
- https://github.com/shadcn-labs/emailcn/tree/main/public/r — built registry files that embed the copied source and deliver it to users through "npx shadcn add"  
- https://github.com/shadcn-labs/emailcn/tree/main/content/docs/components — documentation that displays the copied components  
- https://github.com/shadcn-labs/emailcn/blob/main/registry/email-assets.ts — maps [private] image file names to [private] images' pixel sizes  
  
The rest of the repository serves only to publish and distribute that copied library: the documentation site, the demos that render it, the registry build that packages it, and the themes that restyle it.  
  
THE ORIGINAL WORK  
  
The original work is the Mailviews email components: their HTML/CSS markup and Tailwind CSS source, their demo text, and their image assets.  
  
Every component is publicly viewable, with its rendered HTML, at https://mailviews.com/components, and each has its own anchor URL. Image assets are served from https://assets.mailviews.com/images/components/.  
  
The [private] captured https://mailviews.com/components/marketing/bento-grids on November 21, 2025 ([private]), with the text quoted in Example E already present. The emailcn repository's first commit is dated July 23, 2026.  
  
LICENSE  
  
Mailviews is not open source. Its license (https://mailviews.com/license) prohibits redistributing the components "or any of their derivatives separately from an End Product — whether as source code or design assets". It names as prohibited:  
- "Creating a public repository of your favorite Mailviews Components or Templates, or derivatives of them"  
- "Creating a library of Mailviews Components or Templates, or derivatives of them, and making it available for sale or for free"  
- "Converting a Mailviews Component or Template to another framework and making it available for sale or for free"  
  
emailcn does exactly this. It ports the components to React Email, MJML React and JSX Email, and republishes them under an MIT license.  
  
HOW THE COPYING CAN BE VERIFIED  
  
The line numbers below are pinned to commit 8ff505ea. Permalink prefix:  
https://github.com/shadcn-labs/emailcn/blob/8ff505ea11015b9301802ce59c48a91f9b203cf0/  
  
A. Milestone stats — text and code copied  
Original: https://mailviews.com/components/marketing/stats#milestone-stats  
Copy: registry/bases/react-email/components/marketing/stats/milestone-stats.tsx#L46-L65  
[private]  
  
B. Hero "block overlay"  
Original: https://mailviews.com/components/marketing/heroes#hero-block-overlay  
Copy: registry/bases/react-email/components/marketing/hero/overlay-hero.tsx#L453-L470  
[private]  
  
C. Hero "block with bleed"  
Original: https://mailviews.com/components/marketing/heroes#hero-block-with-bleed  
Copy: overlay-hero.tsx#L775-L789  
[private]  
  
D. Product detail  
Originals: https://mailviews.com/components/ecommerce/product-detail#product-detail-with-two-images-and-details and #product-detail-with-three-images-and-details  
Copies: registry/bases/react-email/components/ecommerce/product-detail/split-product-detail.tsx#L376 and masonry-product-detail.tsx#L449  
[private]  
  
E. Bento grid stats (archived November 21, 2025, see above)  
Original: https://mailviews.com/components/marketing/bento-grids#bento-grid-with-even-split-and-text-stats  
Copy: registry/bases/react-email/components/marketing/bento-grids/bento-stats-grid.tsx#L741-L781  
[private]  
  
F. More verbatim text  
[private]  
  
G. [private] asset file names and their exact pixel sizes  
registry/email-assets.ts#L22-L133 holds 110 rules that map image file names to pixel sizes.  
- 108 of the 110 file names are the names of [private] image files under https://assets.mailviews.com/images/components/. The other 2 are emailcn's own logo.  
- 98 of them give [private] files' exact pixel sizes, including odd ones that could only come from [private] files: hero/split-slanted-bg 2002×2534 (L83), bento-grids/3-bento-lg-1-pad 304×576 (L31), stats/overlay-1 1572×2268 (L103).  
- The file substitutes Unsplash photos so that markup ported from mine renders at [private] dimensions.  
  
H. [private] brand left in the code  
- registry/email-assets.ts#L132-L133 special-cases maizzle-insignia.png.  
- registry/bases/react-email/components/marketing/headers/header-with-logo.tsx#L55-L57 sets [private].  
Maizzle is [private] trademark and the name of the company that owns Mailviews. The Mailviews components use the Maizzle logo as their placeholder brand: https://assets.mailviews.com/images/components/maizzle-insignia.png  
  
I. [private] component names were used verbatim at first  
At the first commit (https://github.com/shadcn-labs/emailcn/tree/5fc7cc1fc4698d7730c5015c24234131c635af5f/public/r/react-email, July 23, 2026), I compared 129 component names taken from 17 Mailviews category pages. 101 of them were used verbatim as emailcn registry item names, for example:  
- order-summary-with-billing-details-and-payment-method-inline  
- bento-grid-with-two-rows-and-3-column-second-row  
These names are the anchor slugs of [private] pages, for example https://mailviews.com/components/ecommerce/order-summary#order-summary-with-billing-details-and-payment-method-inline. The files were renamed in commit 870575ad on July 27, 2026.  
  
SCALE  
  
Across the React Email components, 151 of the 323 distinctive text strings of 25 characters or more are verbatim Mailviews text. This count leaves out lorem ipsum and well-known quotations.  
  
The same text and values appear in the mjml-react and jsx-email ports, in the built registry files under public/r/, and in the docs under content/docs/components/.  
  
FORKS  
  
I reviewed every one of the repository's 9 forks on October 11, 2026. Each one is a copy of the infringing repository, and I believe each is infringing in its entirety.  
  
Eight forks have the same files at the same paths as the parent. In each of them, registry/bases/react-email/components/marketing/hero/overlay-hero.tsx contains "SumUp is a global fintech leader…", and registry/bases/react-email/components/marketing/stats/milestone-stats.tsx contains "Miles traveled":  
SpartanDavie/emailcn, QC1011/emailcn, Croc-2/emailcn, htrivedi0007/emailcn, cmahmud/emailcn, bolasatu/email_generator, ClarkEL-99/emailcn, mustafaskyer/emailcn.  
  
The ninth fork, calebmcclain/emailcn-ui, was made before the July 27, 2026 restructure. The same material is in registry/bases/{react-email,mjml-react,jsx-email}/ui/** and public/r/**. For example, https://github.com/calebmcclain/emailcn-ui/blob/main/registry/bases/react-email/ui/marketing/hero/hero-block-overlay.tsx contains the same SumUp text. 273 of its 562 component files and 303 of its registry files are named with the exact names of Mailviews components.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://mailviews.com/components  
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
The entire repository is infringing  
  
**Identify the full repository URL that is infringing:**  
  
https://github.com/shadcn-labs/emailcn  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
https://github.com/SpartanDavie/emailcn  
https://github.com/QC1011/emailcn  
https://github.com/Croc-2/emailcn  
https://github.com/htrivedi0007/emailcn  
https://github.com/cmahmud/emailcn  
https://github.com/bolasatu/email_generator  
https://github.com/ClarkEL-99/emailcn  
https://github.com/mustafaskyer/emailcn  
https://github.com/calebmcclain/emailcn-ui  
  
**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**  
  
**Is the work licensed under an open source license?**  
  
No  
  
**What would be the best solution for the alleged infringement?**  
  
Reported content must be removed  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
GitHub organization: shadcn-labs ([private])  
Public email: [private]  
Website: [private]  
  
Sole committer: GitHub user [private] ([private]), [private]  
  
The same material is also distributed at [private], which is hosted on [private]. I am sending a separate notice to [private].  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]  
