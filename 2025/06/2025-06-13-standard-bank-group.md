While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

Ownership of copyright: The Standard Bank Group Limited (“SBG”) uses a [private] [private] [private] of the Salesforce (customer services and analytics) source code, which is the subject matter of this complaint. Standard Bank of [private] Limited ("Standard Bank") is the largest operating entity within SBG and is also the owner of the aforesaid Salesforce source code, which is the subject of this complaint. The aforesaid Salesforce source code is original in that it is the result of the independent skill, labour and judgment of Standard Bank's full-time employees and third party software developers, with whom Standard Bank concluded agreements with and exercised full control over at all times. Such full-time employees and third party software developers developed the aforesaid Salesforce source code in the course and scope of their employment and/or contractual engagement with Standard Bank, respectively. In view of the foregoing, the aforesaid Salesforce source code qualifies for copyright protection, as a computer program, in terms of section 11B the South African Copyright Act, 98 of 1978 ("Act"), and to the extent that there are any tables and compilations contained therein, they qualify for protection as literary works under the Act, and Standard Bank is the lawful owner of all of the aforesaid works. The rights in the Salesforce source code have not been assigned, licensed, or otherwise transferred to the alleged infringers. The copyright which subsists in the source code vests exclusive right in Standard Bank to, and to authorise others to, amongst others, reproduce and/or adapt its source code. Standard Bank has not authorised the alleged infringers to perform the aforesaid acts, in relation to its source code.

Authority to act for Standard Bank: I am a [private]r at Standard Bank, within the [private]. I have been mandated and duly authorised to act for Standard Bank in relation to matters of this nature.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

Standard Bank's Salesforce source code [private] [private] contains its Salesforce business functionality and custom applications created to support SBG's business processes, such as sales processes, new business account creation, service processes, bank employee contact information and production system connection configurations. In addition, there is sensitive code information that references the components (i.e. objects and tables) in Salesforce for client card components and client scoring information (public). The codebase also contains test data and mock implementations that may include or reflect actual customer data patterns. Please see detailed breakdown below.

1. API credentials - Client ID, Client Secret, API Gateway and Interaction ID was found. The foregoing credentials were valid production credentials for the Standard Bank API Gateway at the time of exposure.  
2. SSL certificates with private key information were discovered in the infringers' files. These could be used to impersonate or intercept secure communications.
3. Endpoint Information: There are exposed API endpoints to a production environment.  
4. Connected App OAuth Credentials: Multiple OAuth consumer keys (8) were discovered in connected app configurations.  
5. Several email addresses for system administrators and technical contacts were exposed.

**If the original work referenced above is available online, please provide a URL.**

https://github.com/sheheryar25/standardbank  
https://github.com/aneelaoad/standard-bank  
https://github.com/pgajek2/standardbank (a fork of sheheryar25's repository)

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/sheheryar25/standardbank  
https://github.com/aneelaoad/standard-bank  
https://github.com/pgajek2/standardbank

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Standard Bank makes use of Salesforce contractors and has agreements in place with third party contractors that include confidentiality obligations and prohibitions against intellectual property infringement, whether for the third party's own benefit or that of another party. In addition, the third party contractors are onboarded onto Standard Bank’s network with a Standard Bank email address. The third parties are required to follow a logical access management process to get access to Salesforce (profiles, roles and permission sets), subject to line manager approval. Standard Bank has audit trails in place for Salesforce. Third parties (such as software developers) require login credentials in order to access Salesforce and its associated software tools (e.g. [private]). Standard Bank's Salesforce business area, uses [private] for the management and storage of its Salesforce code. One of the business areas within Standard Bank also uses [private] for its code repository. In this regard, there is also a logical access management process to gain access to Standard Bank's [private] [private] account, with line manager approval. The aforesaid development teams also have access to Standard Bank's proprietary code and configuration, which is stored in Standard Bank's [private] [private] on [private]. The infringers (formerly, third party contractors of Standard Bank) used GitHub in their [private] capacity and made copies of Standard Bank's source code and published same on the infringers' public GitHub accounts.

**How is the accused project designed to circumvent your technological protection measures?**

Our internal investigations suggest that the alleged infringers were previously employed by Standard Bank's third party vendors, with whom Standard Bank concluded agreements with for the provision of software development services. During this time, the alleged infringers would have obtained the requisite security credentials in order to access Standard Bank's private code repository on [private] as well as Standard Bank's internal IT systems. It is during such time that the alleged infringers, unbeknownst to Standard Bank and without Standard Bank's approval, reproduced (i.e., made a slavish copy of) Standard Bank's Salesforce codebase and uploaded same onto their personal GitHub repositories (using their personal gmail accounts), and made same publicly available.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/pgajek2/standardbank

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

Primary contributor: [private] ([private])  
Author: [private] and [private]    
Fork: [private]: [private] and [private]

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
