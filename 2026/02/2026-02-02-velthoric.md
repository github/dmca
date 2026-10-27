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

I am the [private] [private] and [private] [private] of the Velthoric project, a Minecraft physics mod. [private] wrote all of the buoyancy system code in question from scratch. The project is hosted publicly on GitHub under [private] [private] [private] and is licensed under LGPL 3.0.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The infringed work is a buoyancy physics system written in Java, consisting of five files: VxBuoyancyBroadPhase.java, VxBuoyancyNarrowPhase.java, VxBuoyancyDataStore.java, VxBuoyancyManager.java, and VxFluidType.java. These files implement a two-phase fluid detection and force application system using a Structure of Arrays data layout, a triple-buffered data store with AtomicReference for thread safety, and a GC-optimized voxel scan algorithm. The infringing repository copies the same architecture, the same field names (waterCenterX, waterCenterZ, flowX, flowY, flowZ, surfaceHeights, areaFractions), the same algorithm logic for finding fluid surfaces both upward and downward, the same flow direction calculation, the same thread-local temporary object pattern, and identical physical constants (buoyancyFactor 1.1f and 2.5f, linearDrag 0.5f and 5.0f, angularDrag 0.05f and 2.0f, MAX scan value of 16). The copied code is not a reimplementation but a direct structural and logical copy with only surface-level renaming applied.

**If the original work referenced above is available online, please provide a URL.**

https://github.com/xI-Mx-Ix/Velthoric

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

https://github.com/ladakx/Inertia/blob/master/inertia-core/src/main/java/com/ladakx/inertia/physics/world/buoyancy/BuoyancyBroadPhase.java  
https://github.com/ladakx/Inertia/blob/master/inertia-core/src/main/java/com/ladakx/inertia/physics/world/buoyancy/BuoyancyNarrowPhase.java  
https://github.com/ladakx/Inertia/blob/master/inertia-core/src/main/java/com/ladakx/inertia/physics/world/buoyancy/BuoyancyDataStore.java  
https://github.com/ladakx/Inertia/blob/master/inertia-core/src/main/java/com/ladakx/inertia/physics/world/buoyancy/BuoyancyManager.java  
https://github.com/ladakx/Inertia/blob/master/inertia-core/src/main/java/com/ladakx/inertia/physics/world/buoyancy/FluidType.java

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

No

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

Yes

**Which license?**

other

**How do you believe the license is being violated?**

The infringing repository copies code from [private] LGPL 3.0 licensed project without including the required copyright notice, without crediting the original author, and without indicating that the code is derived from [private] work. LGPL 3.0 requires that all copies and derivative works retain the original copyright notice and author attribution. None of this is present in the infringing repository.

**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**

The infringing files should be removed entirely. If the author wishes to use the code legitimately under LGPL 3.0, they must add proper attribution to the original author (xI-Mx-Ix), include the LGPL 3.0 license notice in each affected file, and clearly indicate that the files are based on or derived from the Velthoric project.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

GitHub username: ladakx  
Repository: https://github.com/ladakx/Inertia

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
