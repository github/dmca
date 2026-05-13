While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

One or more repositories in this DMCA takedown notice has been processed in accordance with GitHub's prohibition on sharing unauthorized product licensing keys, software for generating unauthorized product licensing keys, and/or software for bypassing checks for product licensing keys.

You can learn more in [GitHub's Acceptable Use Policies](https://docs.github.com/en/github/site-policy/github-acceptable-use-policies).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am an authorized representative of Jeppesen ForeFlight, the copyright owner of the proprietary Jeppesen Mobile TC software.

Jeppesen ForeFlight owns all intellectual property rights in the application, including its source code, compiled binaries, and associated aviation data processing logic. The material published in the referenced repository წარმოადგენს a decompiled and unauthorized derivative of this proprietary software.

I am authorized to act on behalf of Jeppesen ForeFlight in matters relating to the protection and enforcement of its intellectual property rights, including submitting DMCA takedown requests for unauthorized reproduction, reverse engineering, and distribution of its software.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The original copyrighted work is the Jeppesen Mobile TC (Terminal Charts) application, a proprietary aviation software product developed and owned by Jeppesen ForeFlight.

Jeppesen Mobile TC is a commercial, licensed application designed for use by professional pilots and aviation operators. It provides access to digitally rendered terminal charts, approach plates, airport diagrams, and navigation data, along with supporting features for flight operations. The application includes proprietary components such as:

Source code and compiled binaries that implement chart rendering, data parsing, and application logic  
Custom data handling mechanisms for processing regulated aeronautical datasets  
Authentication and licensing controls tied to subscription-based aviation services  
User interface design, workflows, and functional architecture specific to aviation use cases  
Integration logic for interoperability with other Jeppesen and ForeFlight aviation systems

The software is distributed exclusively through authorized channels (e.g., official app stores and licensed enterprise distribution) and is protected under applicable copyright laws and licensing agreements. Access to its full functionality and underlying data is restricted to authorized users under valid commercial agreements.

The repository identified in this request contains a decompiled and reverse-engineered version of this application, which reproduces substantial portions of the original copyrighted work without authorization. This includes reconstructed source code and structural elements derived directly from the original Jeppesen Mobile TC application.

**If the original work referenced above is available online, please provide a URL.**

https://github.com/zeykafx/Jeppesen-Mobile-TC_v1.2.0.13-decompiled

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/zeykafx/Jeppesen-Mobile-TC_v1.2.0.13-decompiled

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Jeppesen ForeFlight employs multiple layered technological measures to control and protect access to its proprietary software, including the Jeppesen Mobile TC application:

1. Authentication and Account-Based Access Control  
Access to the application and its underlying aviation data is restricted to authenticated users with valid Jeppesen ForeFlight accounts. User identity is verified through secure login mechanisms, and access is governed by role and subscription entitlements.

2. Subscription and Licensing Enforcement  
The application operates on a commercial licensing model. Access to features and aeronautical datasets is controlled through backend entitlement systems that validate active subscriptions and enforce usage restrictions.

3. Application Integrity and Anti-Tampering Controls  
The distributed application binaries are protected using platform-level security controls such as code signing and integrity verification. These mechanisms are designed to prevent unauthorized modification, reverse engineering, and execution of tampered versions.

4. Secure Data Delivery and Encryption  
Aeronautical data and related services are delivered over encrypted channels, with additional controls to ensure that sensitive datasets are only accessible to authorized and validated clients.

5. Backend Service Authorization Controls  
Critical functionality, including data retrieval and synchronization, is enforced server-side. Requests are validated against authentication tokens and entitlement checks, preventing unauthorized clients from accessing protected resources.

6. Platform Distribution Controls  
The application is distributed only through authorized platforms and controlled enterprise channels, which enforce additional safeguards such as application signing, sandboxing, and device-level protections.

These measures collectively ensure that only authorized users with valid credentials and entitlements can access the copyrighted software and associated aviation data.

The decompiled repository in question represents a circumvention of these technological protection measures, as it reconstructs and exposes protected application logic outside the authorized execution environment, thereby undermining access controls and enabling potential misuse.

**How is the accused project designed to circumvent your technological protection measures?**

The accused repository is designed to circumvent the technological protection measures implemented by Jeppesen ForeFlight by distributing a decompiled and reverse-engineered version of the Jeppesen Mobile TC application.

Specifically, the project undermines these protections in the following ways:

1. Circumvention of Code Integrity and Distribution Controls  
The application has been decompiled from its original, signed binary form into reconstructed source code. This process bypasses platform-enforced protections such as code signing, integrity verification, and controlled distribution channels, enabling the software to be analyzed and potentially recompiled outside authorized environments.

2. Exposure of Protected Application Logic  
Decompilation reveals internal logic, workflows, and structural components that are intentionally protected within compiled binaries. This includes how the application processes aeronautical data, interacts with backend services, and enforces access restrictions. Making this logic publicly available defeats the purpose of those protections.

3. Facilitation of Reverse Engineering of Access Controls  
By exposing authentication flows, API interaction patterns, and entitlement checks, the repository lowers the barrier for malicious actors to:

Identify how access to subscription-based services is enforced  
Attempt to replicate or bypass client-side controls  
Develop unauthorized clients or modified versions of the application

4. Enabling Unauthorized Modification and Redistribution  
The reconstructed code can be modified, recompiled, and redistributed without authorization. This creates a pathway to produce altered versions of the application that may disable licensing checks, remove security controls, or misrepresent the integrity of aviation data workflows.

5. Undermining Layered Security Architecture  
Jeppesen ForeFlight relies on a combination of client-side protections and server-side enforcement. The decompiled project weakens this model by exposing implementation details that are not intended to be publicly accessible, thereby increasing the risk of targeted abuse against backend systems.

In summary, the repository does not merely display information, it materially enables the bypassing and weakening of technical safeguards that control access to proprietary aviation software and data. This constitutes a circumvention of technological protection measures under applicable copyright and anti-circumvention laws.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
