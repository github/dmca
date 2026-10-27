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

[private] the [private], [private], and [private] copyright holder of the NeighborhoodAid project, which I built entirely myself under the MIT License. I hold the copyright to the entire codebase, including all associated files and intellectual property. The project was developed over 364 commits and more than 57,000 lines of code, and it is protected under the MIT License as of September 26, 2024 (Commit ID: d43e918).

The original repository, containing the MIT License, is available publicly, as is the live application:

Original Repository: https://github.com/mirasoldavila13/NeighborhoodAid  
Live Application: https://neighborhoodaid.onrender.com  
License: https://github.com/mirasoldavila13/NeighborhoodAid/blob/main/LICENSE

As the [private] copyright owner, I am authorized to act on behalf of this work and formally request the removal of the infringing content.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed. If possible, include a URL to where it is posted online.**

The original copyrighted work consists of a web application titled NeighborhoodAid, which is designed to facilitate community reporting and issue tracking. This application includes various features such as user registration, authentication, and reporting functionalities.

The codebase is organized into multiple components and modules, including middleware for handling requests, controllers for managing business logic, and services for interacting with APIs. The application is built using technologies such as JavaScript, React, and Node.js, and it adheres to best practices for security and user experience.

The project is publicly available at the following URL: https://github.com/mirasoldavila13/NeighborhoodAid.

This repository is licensed under the MIT License, which permits reuse of the code as long as proper attribution is given to the original author.
Instances of Code Duplication

The following files from NeighborhoodAid have been copied without permission in the repository dncBot-fullDatabase:

client/src/App.tsx  
Original Commit: 9936d42 on October 3, 2024  
Infringement Commit: a3be74bb80dd0e35db433cc85c8910f767f4913d on November 3, 2024  

server/middleware/validationMiddleware.js  
Original Commit: fc8fa5a26d6b9c092917fbd6cc2e9906d0299957 on October 2, 2024  
Infringement Commit: a3be74bb80dd0e35db433cc85c8910f767f4913d on November 3, 2024

server/middleware/authMiddleware.js  
Original Commit: c4238e5c669e5e4d0f25c670281a461dadb06741 on October 6, 2024  
Infringement Commit: a3be74bb80dd0e35db433cc85c8910f767f4913d on November 3, 2024

server/controllers/authController.js    
Original Commit: 7e08854dbcc0a13b7c1528b55ff4748822d0c770 on October 10, 2024  
Infringement Commit: a3be74bb80dd0e35db433cc85c8910f767f4913d on November 3, 2024

client/src/services/authService.ts    
Original Commit: 90c65cafefc5769a9237cfa6a9fc1598d2aadb9c on October 14, 2024  
Infringement Commit: a3be74bb80dd0e35db433cc85c8910f767f4913d on November 3, 2024

client/src/pages/UserRegistration.tsx  
Original Commit: b2c8a9c870b526c4e27dfb319a460f9ce4b541e9 on October 17, 2024  
Infringement Commit: a3be74bb80dd0e35db433cc85c8910f767f4913d on November 3, 2024
 
client/src/pages/Login.tsx  
Original Commit: 95036c89748c7d40a28635bfe8ab8c57ec53d5a9 on October 9, 2024  
Infringement Commit: a3be74bb80dd0e35db433cc85c8910f767f4913d on November 3, 2024

**What files should be taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**

I request the deletion of all code copied from [private] original project, NeighborhoodAid, in the repository dncBot-fullDatabase owned by [private]. The following files contain significant portions of [private] original work without proper attribution:

client/src/App.tsx  
URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase/blob/main/client/src/App.tsx  
Reason: This file includes a catch-all route that is directly copied from NeighborhoodAid, demonstrating a lack of original contribution.

server/middleware/validationMiddleware.js  
URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase/blob/main/server/middleware/validationMiddleware.js  
Reason: The content validation logic, including the specific error message "Content cannot be empty," is duplicated without any changes.

server/middleware/authMiddleware.js  
URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase/blob/main/server/middleware/authMiddleware.js  
Reason: This file replicates [private] JWT authentication error-handling logic, including identical error messages and conditions.

server/controllers/authController.js  
URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase/blob/main/server/controllers/authController.js  
Reason: Both files contain identical registration and login logic, with no modifications, clearly showing that the functionality was copied.

client/src/services/authService.ts  
URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase/blob/main/client/src/services/authService.ts  
Reason: This file retains identical methods for handling registration, login, and tokens, mirroring [private] original work.

client/src/pages/UserRegistration.tsx  
URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase/blob/main/client/src/pages/UserRegistration.tsx  
Reason: The component logic, state management, and error handling are nearly identical, with some original code left as comments, indicating minimal alteration.

client/src/pages/Login.tsx  
URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase/blob/main/client/src/pages/Login.tsx  
Reason: The structure, logic, and error handling for login are identical to [private] implementation, showing direct copying.

This is the second time I am addressing unauthorized copying of [private] project, NeighborhoodAid, by [private]. I have previously requested the removal of copied code from his earlier repository, [private], which is documented here: [private].

Unfortunately, I was unable to fork his current repository, dncBot-fullDatabase, due to restrictions he has in place. As a result, I cannot submit an issue or pull request to formally request the removal of the copied code. This limitation makes it challenging to address this matter directly with him.

I respectfully request that [private] remove the above files from the dncBot-fullDatabase repository to resolve this copyright infringement issue. Additionally, if the copied code cannot be adequately modified to comply with the MIT License requirements for attribution, I ask that the entire repository be taken down.

Repository URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

No

**<a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">Have you searched for any forks</a> of the allegedly infringing files or repositories? Each fork is a distinct repository and must be identified separately if you believe it is infringing and wish to have it taken down.**

I have searched for any forks of the allegedly infringing repository. It is important to note that [private] previous repository, [private], where he initially copied code from [private] project NeighborhoodAid, is no longer publicly accessible. However, I still have access to my fork of that repository, which can be found at the following URL: [private]

Since GitHub has not provided an update regarding the specific DMCA takedown submitted for [private], I have retained my forked repository. Currently, the repository in question is dncBot-fullDatabase, located at https://github.com/IsaiahSkidmore/dncBot-fullDatabase, which contains multiple instances of copied code from NeighborhoodAid without proper attribution.

**Is the work licensed under an open source license?**

Yes

**Which license?**

The NeighborhoodAid project is licensed under the MIT License, an open-source license that allows reuse with proper attribution and under the same license terms. The MIT License was added to the repository on September 26, 2024, with the commit ID: d43e918  https://github.com/mirasoldavila13/NeighborhoodAid/blob/99a02afa5767c6bd88b3a6dc06ea2657e51ee86b/LICENSE .

**How do you believe the license is being violated?**

The NeighborhoodAid project is licensed under the MIT License, which mandates proper attribution to the original creator and retention of the original license in any reused work. [private] actions in his repository dncBot-fullDatabase demonstrate a continued disregard for these conditions, highlighting a troubling pattern in his approach to using my work without proper acknowledgment. Specifically, this misuse of my code constitutes:

Violation of the MIT License: [private] did not retain the original license in his project, which misrepresents the origin and ownership of the work. This omission undermines the legal framework that allows others to use [private] code.

Plagiarism: While the extent of code duplication in dncBot-fullDatabase is less than in his previous [private], [private], he has still copied significant portions of my work, including core functionalities such as middleware, services, and client-side components. This reflects a continued attempt to present my work as his own without proper credit.

Disregard for Open-Source Ethics: The MIT License exists to foster collaboration, transparency, and fairness. [private] actions undermine these principles by reusing my code without acknowledgment and misrepresenting its origin, which is contrary to the spirit of open-source software.

Impact on the Coding Community: Plagiarism and license violations erode trust within the software development community. Proper attribution ensures creators receive recognition for their contributions and helps maintain a transparent and ethical environment. Violating these norms disrespects both individual creators and the broader coding ecosystem.

[private] was present during a [private] where I explicitly added the MIT License to the NeighborhoodAid repository, indicating he was fully aware of the license’s requirements. Despite this awareness, he has continued to disregard those requirements by copying my code into his projects without acknowledgment.

Additionally, he has set his repository permissions so that I am unable to fork it or submit an issue. This deliberate restriction further illustrates his awareness of the copyright issues and his intention to avoid accountability.

This is not the first instance of such behavior. I previously filed a DMCA Takedown Notice regarding his earlier [private], [private], where he also copied my work without permission. The fact that he has again replicated portions of my code in dncBot-fullDatabase, albeit to a lesser extent, suggests a recurring issue rather than a simple oversight.

[private] provided [private] with the opportunity to create a solo project to demonstrate his understanding of the coursework instead of receiving a zero for the group assignment. However, instead of developing his original code, he has reused my work, violating both the legal requirements of the MIT License and the ethical standards of academic integrity.

This ongoing behavior not only breaches the licensing agreement but also constitutes plagiarism, a serious offense in both academic and professional settings. By violating the license, plagiarizing my code, and failing to respect the values of the coding community, [private] actions have caused harm beyond the scope of this project. These violations must be addressed to protect the integrity of the open-source ecosystem and ensure that proper credit is given where it is due.

**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**

To bring the project dncBot-fullDatabase into compliance with the MIT License, several changes must be made to ensure proper attribution and adherence to licensing requirements. I have taken the following steps regarding [private] project, NeighborhoodAid, to maintain professionalism and transparency:

Repository Accessibility: Although [private] initially made the NeighborhoodAid [private] to prevent unauthorized copying, I was required to make it public again for [private] on [private]. [private] must have access to the repository to evaluate my work, which means I am currently unable to keep it [private].

Versioning and Attribution: I have added a 1.0 Contributions section to the README, recognizing [private] and the other initial team members for their early-stage involvement. To further ensure clarity, I added the following Attribution Watermark:

NeighborhoodAid initially originated as a group project. However, due to team availability and shifting responsibilities, the project has since been further developed and expanded individually by [private]. All subsequent feature implementations, optimizations, and refinements were led solely by [private] to enhance the platform’s functionality, scalability, and user experience.

Version 2.0 Contributions: I documented the transition to solo development in the README under Version 2.0 Contributions:

Overview: The development of Version 2.0 of NeighborhoodAid was completed under the leadership of [private], building on the solid foundation established during the early stages of the project. While [private], [private], and [private] made early contributions during Version 1.0, they have since shifted focus to other commitments. Their efforts provided helpful starting points for the project, and we appreciate their involvement in the initial stages. All new features, improvements, and refinements in Version 2.0 were designed, developed, and implemented solely by [private]. This release introduces key enhancements that significantly expand the platform’s functionality and usability, laying the groundwork for future developments.

Commit Verification: To ensure that all subsequent contributions are traceable and authentic, [private] used the -S flag on every commit after the project transitioned to solo development. This ensures all commits are signed and verified as [private].

Pull Request on Infringing Repository: I have already submitted a pull request on [private] repository, asking for the removal of the copied code and proper attribution to NeighborhoodAid. Despite this effort, [private] has not taken down the infringing content or responded to my request.

Exclusion of the Render Link: To prevent misattribution or confusion, I deliberately did not include the Render deployment link in the repository, ensuring that no one could falsely claim the live platform as their own.

Best Solution for the Alleged Infringement

To ensure compliance with the MIT License and respect for original work, [private] must take the following actions:

Remove all copied code from NeighborhoodAid in the dncBot-fullDatabase repository.  
Provide proper attribution for any reused code and include the MIT License in the repository.

This is the second time I am addressing unauthorized copying of [private] project, NeighborhoodAid, by [private]. I previously requested the removal of copied code from his earlier [private], [private], which is documented here: [private].

Unfortunately, I was unable to fork his current repository, dncBot-fullDatabase, due to restrictions he has in place. As a result, I cannot submit an issue or pull request to formally request the removal of the copied code. This limitation makes it challenging to address this matter directly with him.

I respectfully request that [private] remove the above files from the dncBot-fullDatabase repository to resolve this copyright infringement issue. Additionally, if the copied code cannot be adequately modified to comply with the MIT License requirements for attribution, I ask that the entire repository be taken down.

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

Name: [private]  
Email: [private]  
GitHub Username: IsaiahSkidmore  
Repository URL: https://github.com/IsaiahSkidmore/dncBot-fullDatabase

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]  
[private]  

**Please type your full legal name below to sign this request.**

[private]  
