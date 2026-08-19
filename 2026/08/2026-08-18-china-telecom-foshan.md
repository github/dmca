1. Are you the copyright holder or authorized to act on the copyright owner's behalf?  
Yes, I am authorized to act on the copyright owner's behalf.  
  
2. Are you submitting a revised DMCA notice after GitHub Trust & Safety requested changes?  
Yes.  
  
3. Does your claim involve content on GitHub or npm.js?  
GitHub.  
  
4. Nature of copyright ownership and authorization  
I am an employee in charge of [private] at China Telecom Corporation Limited Foshan Branch ("China Telecom Foshan"). The LtePlatform codebase was developed by employees of China Telecom Foshan during the course of their employment as work-for-hire, and the copyright is owned by China Telecom Foshan. I am authorized to submit this takedown notice on behalf of the company.  
  
5. Detailed description of the original copyrighted work  
The LtePlatform repository is a proprietary .NET / ASP.NET software solution for LTE wireless network optimization, developed internally by China Telecom Foshan employees. It comprises 19 sub-projects (9 application projects and 10 test projects) written in C#, including:  
  
LtePlatform — the main ASP.NET MVC / Web API 2 application (controllers, views, authentication)  
  
Lte.Domain — domain models and data infrastructure  
  
Lte.MySqlFramework — MySQL entity and repository implementations  
  
Lte.Parameters — SQL Server and MongoDB data access layer  
  
Lte.Evaluations — application/data services layer  
  
TraceParser — LTE signaling data parser (hex signaling decoder for Huawei/ZTE/Ericsson)  
  
Lte.Auxilary — Python scripts for backend data preprocessing (MR data collection, parsing, and database insertion)  
  
This code was never licensed under any open-source license and was never intended for public distribution.  
  
6. If the original work is available online, please provide a URL  
The original repository was hosted at [private] (the README files in the forks still contain documentation links pointing to this original URL). The original repository was deleted long ago because it was uploaded without proper authorization. It is no longer publicly available, which is precisely why we are requesting that the surviving forked copies also be removed.  
  
7. Is the entire repository infringing, or specific files?  
The entire repository is infringing. Every file in each of the reported repositories is a copy of proprietary code that belongs to China Telecom Foshan. No portion of these repositories was published under an open-source license, and none of the fork authors have any authorization to possess or redistribute this code.  
  
The following are specific, representative examples of the types of infringing and sensitive content found in these repositories. These examples are illustrative — the entire repository in each case should be removed.  
  
7a. Plaintext database credentials (in LtePlatform/Web.config)  
  
The file Web.config (present in all five repositories) contains ** plaintext database connection strings with live usernames and passwords** for both internal SQL Server and MySQL databases:  
  
[private]  
  
Direct file links in each repository:  
  
https://github.com/xuhaoa/LtePlatform/blob/master/LtePlatform/Web.config  
  
https://github.com/xc-link/LtePlatform/blob/master/LtePlatform/Web.config  
  
https://github.com/PowerDG/LtePlatform/blob/master/LtePlatform/Web.config  
  
https://github.com/sky016685/LtePlatform/blob/master/LtePlatform/Web.config  
  
https://github.com/chandusekhar/LtePlatform/blob/master/LtePlatform/Web.config  
  
A second configuration file, Web.20170818.config, contains additional internal database credentials with a different set of internal IP addresses and passwords:  
  
https://github.com/xuhaoa/LtePlatform/blob/master/LtePlatform/Web.20170818.config  
  
https://github.com/xc-link/LtePlatform/blob/master/LtePlatform/Web.20170818.config  
  
7b. Internal network infrastructure information (in README.md)  
  
The README.md file in each repository exposes internal network addresses of China Telecom Foshan's production systems:  
  
[private]  
  
Direct links:  
  
https://github.com/xuhaoa/LtePlatform/blob/master/README.md  
  
https://github.com/xc-link/LtePlatform/blob/master/README.md  
  
https://github.com/PowerDG/LtePlatform/blob/master/README.md  
  
https://github.com/sky016685/LtePlatform/blob/master/README.md  
  
https://github.com/chandusekhar/LtePlatform/blob/master/README.md  
  
7c. Internal database schema and Telecom operational logic  
  
The Lte.Parameters project document (README.md / Databases.md) describes the internal data architecture of China Telecom's network management systems, including:  
  
LTE and CDMA base station (eNodeB/BTS) data structures  
  
Daily KPI indicators received from the provincial center  
  
MR (Measurement Report) data processing pipelines  
  
Work order and alarm data from the network management system  
  
Campus network infrastructure data from a 2015 project  
  
This information reveals internal operational details of China Telecom's telecommunications infrastructure.  
  
7d. Proprietary source code  
  
All C# source files, Python scripts, JavaScript/AngularJS frontend code, and database migration files in each repository are proprietary works owned by China Telecom Foshan. There is no open-source license file in any of these repositories.  
  
8. Nature of the infringement — why this is NOT a routine fork  
GitHub's response noted that "simply forking a public repository is generally not considered copyright infringement" under Section D5 of the Terms of Service. I respectfully clarify that this case is fundamentally different from a routine fork for the following reasons:  
  
The original repository was never lawfully public. The code was uploaded to GitHub by an employee without authorization from China Telecom Foshan. The original repository at [private] was deleted once the unauthorized upload was discovered. The fact that the original was briefly public due to an unauthorized act does not legitimize the continued distribution of the code.  
  
The code contains trade secrets and sensitive credentials. The Web.config files contain plaintext database usernames and passwords for China Telecom's internal databases, along with internal network IP addresses. These are trade secrets under applicable law. Their continued public availability constitutes an ongoing security risk to China Telecom's infrastructure.  
  
No open-source license exists. The code was never licensed under any open-source or permissive license. The absence of a LICENSE file confirms that no grant of rights was ever made to the public. Therefore, the "forking a public repo" provision of ToS Section D5 does not apply — there was never a valid grant of a license to copy or redistribute.  
  
The fork authors have no authorization. None of the five fork authors are or were employees of China Telecom Foshan, and none have any authorization to possess or redistribute this proprietary code.  
  
In summary: the original upload was unauthorized, the original repository has been removed, and the surviving forks continue to distribute proprietary code containing trade secrets (database credentials, internal network addresses, and Telecom operational data) that were never intended for public disclosure.  
  
9. Identify the full repository URLs that are infringing  
The following five repositories are identical or substantially identical copies of the proprietary LtePlatform codebase:  
  
https://github.com/xuhaoa/LtePlatform — 3,384 commits; this is the parent repository from which the other four were forked. Its README explicitly states "LTE optimization tools for China Telecom" and exposes an internal IP address.  
  
https://github.com/xc-link/LtePlatform — 3,279 commits; forked from xuhaoa/LtePlatform. Contains the same Web.config with plaintext credentials and the README exposing internal IPs.  
  
https://github.com/PowerDG/LtePlatform — 1,346 commits; forked from xuhaoa/LtePlatform. Contains the same Web.config with plaintext credentials and a README exposing a public-facing IP of the platform.  
  
https://github.com/sky016685/LtePlatform — 1,346 commits; forked from xuhaoa/LtePlatform. Identical content to PowerDG fork.  
  
https://github.com/chandusekhar/LtePlatform — 1,346 commits; forked from xuhaoa/LtePlatform. Identical content to PowerDG fork.  
  
10. Fork identification  
Each fork is a distinct repository and is identified separately above. All five repositories contain the same infringing content, including:  
  
LtePlatform/Web.config — plaintext database credentials  
  
LtePlatform/Web.20170818.config — additional plaintext database credentials (in xuhaoa and xc-link forks)  
  
README.md — internal network IP addresses  
  
All proprietary source code files  
  
11. Is the work licensed under an open source license?  
No. The work has never been licensed under any open-source license.  
  
12. What would be the best solution for the alleged infringement?  
Reported content must be removed.  
  
13. Do you have the alleged infringer's contact information?  
No.  
  
14. Technological measures to control access  
No.  
  
15. Good faith belief statement  
I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.  
  
16. Fair use consideration  
I have taken fair use into consideration. The repositories reproduce the entire proprietary codebase verbatim, including trade secrets (database credentials and internal network addresses). This is not a transformative use, commentary, criticism, or parody — it is wholesale copying of proprietary software. No fair use defense applies.  
  
17. Accuracy statement  
I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.  
  
18. I have read and understand GitHub's Guide to Submitting a DMCA Takedown Notice  
Yes.  
  
19. Contact information  
Name: [private]  
  
Phone: [private]  
  
Organization: China Telecom Corporation Limited [private]
  
Email: [private] (via GitHub)  
  
20. Signature  
[private]  
