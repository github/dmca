Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

Note: Because the reported network that contained the allegedly infringing content was larger than one hundred (100) repositories, and the submitter alleged that all or most of the forks were infringing to the same extent as the parent repository, GitHub processed the takedown notice against the entire network of 198 repositories, inclusive of the parent repository. 

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am the [private] [private] and copyright owner of the ExerciseDB dataset, which was developed and curated by [private] through [private] company [private]. I hold all exclusive rights to the dataset’s structure, metadata, and original exercise descriptions, including the text of exercise instructions, field naming conventions, and organization of the database.

[private] operate and license this dataset commercially through [private] official website https://www.exercisedb.io￼ and API service ([private]). I have not granted permission for any redistribution, publication, or derivative work beyond the terms described in my End User License Agreement (EULA) and Terms of Use.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The original copyrighted work is the ExerciseDB dataset, a comprehensive structured database of exercise information authored and maintained by [private]  ([private]).

It includes over 1,300 uniquely formatted entries describing physical exercises, each containing fields such as:  
• id, name, target, bodyPart, equipment, secondaryMuscles, and instructions  
• A proprietary collection of descriptive text I authored for each exercise, detailing proper form and movement steps.

This dataset was developed over several years and is distributed under commercial license via:  
• Official website: https://www.exercisedb.io  
• Documentation: https://edb-docs.up.railway.app￼ 
• API listing: [private]

The infringing repository reproduces [private] dataset’s structure and contents including identical exercise names, metadata, and near-verbatim instruction text (after removal of trivial “Step:N” prefixes) in the file:  
https://github.com/ExerciseDB/exercisedb-api/blob/main/src/data/exercises.json

**If the original work referenced above is available online, please provide a URL.**

As referenced in the previous section as well:

Primary website: https://www.exercisedb.io￼ 
Official documentation: https://edb-docs.up.railway.app  
API listing ([private]) : [private]

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

https://github.com/ExerciseDB/exercisedb-api/blob/main/src/data/exercises.json  
https://github.com/ExerciseDB/exercisedb-api/tree/main/media

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Access to my copyrighted ExerciseDB dataset is controlled through a combination of authentication, rate-limiting, and license-based distribution mechanisms.
• API authentication: All access to the ExerciseDB data is gated through authenticated API endpoints that require valid API keys issued via my RapidAPI listing.  
• Usage tracking and rate limiting: Every request is logged and rate-limited according to subscription tier, preventing unauthorized bulk extraction.
• License enforcement: The full dataset is never distributed publicly; it is served authenticated and verified means to licensed users who agree to the EULA and Terms of Use.  
• File access control: Source data and GIF media files are stored in private, access-controlled environments on Railway servers and MongoDB GridFS buckets, not in any public repository.

**How is the accused project designed to circumvent your technological protection measures?**

The accused project publicly republishes the full dataset and media in a downloadable JSON and media folder, thereby bypassing all of our access controls and license enforcement. [private] ExerciseDB dataset is protected behind authenticated API endpoints (API keys), rate limits, usage logging, and private storage of GIF/media; the accused project avoids these protections by hosting a copy of the data in a public GitHub repository and serving the media directly (media folder/URLs), which allows unrestricted bulk access without API keys or subscription.

In addition, the repository mirrors our proprietary schema and instruction text (including near-verbatim instructional steps), and in several instances the only difference is a trivial “Step:N” prefix added to each instruction line or an object property pluralized which is an obfuscation tactic that does not alter the substantive text but may be intended to mask direct copying. Because they publish the data and media openly (and claim an AGPL license), users can download, mirror, or deploy the full dataset and GIFs without ever interacting with our authenticated API or agreeing to our EULA; this design directly defeats our technological measures (authentication, rate-limiting, and private storage) and enables unrestricted redistribution and reuse of copyrighted material that is supposed to be accessible only to licensed users.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**

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
