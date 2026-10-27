Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

The Client is the copyright owner of BYD Auto App (including domestic and overseas versions), and now entrusts the [private] [private] as the agent of BYD Auto Industry Co., Ltd. to handle the intellectual property infringement complaints on Github platform on behalf of BYD Automobile Industry Co., Lt

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

BYD Auto App (domestic and overseas versions) has obtained the computer software copyright registration certificate from the National Copyright Administration of the [private], and BYD has obtained all the rights of the software. The specific registration number of the copyright is: overseas version ([private]), domestic version ([private]).

**If the original work referenced above is available online, please provide a URL.**

https://github.com/Niek/BYD-re

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/Niek/BYD-re

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

1) BYD Auto App (domestic and overseas versions) has obtained the computer software copyright registration certificate from the National Copyright [private], and BYD has obtained all the rights of the software. The specific registration number of the copyright is: overseas version ([private]), domestic version ([private]).  
2) Up to now, BYD has registered a total of 153 copyrights (copyrights), and the total number of authorized patents in the world is over 42,000.With the core concept of "technology is king and innovation is the foundation", BYD regards intellectual property protection as the core strategy of technological innovation and global competition, and constructs a full-chain intellectual property protection system from R & D layout, right confirmation, management, rights protection to standards and openness, so as to build a strong technical moat in all aspects such as patents, copyrights and trademarks.BYD is committed to establishing a series of management systems to protect the company's intellectual property rights and strictly prevent the disclosure of intellectual property information, including but not limited to https authentication, token authentication, reinforcement technology, white box key and other technical measures.  
3) The domestic APP has obtained the three-level certification of network security protection issued by the State Administration of Market Supervision and Administration and the [private].

**How is the accused project designed to circumvent your technological protection measures?**

1) White-box encryption shell breached (WBSK/Bangcle)The project extracted and reconstructed the encryption logic of the overseas version of Bangcle and the domestic version of WBSK.Through hard-coded Lookup Tables, an attacker can unravel the outer Envelope encryption, exposing internal business logic fields.node decompile.js http-dec '<payload>'  
2) Leakage of static key and signature algorithmIt is found that the API uses the signature mechanism (Checkcode) based on'MD5 'and'SHA-256', and there is a static configuration key (CONFIG _ KEY).Attackers can construct legitimate request packets and forge App behavior.sign: sha1Mixed(buildSignString(..., md5(password)))  
3) MQTT real-time telemetry monitoringBYD uses EMQ agents to push real-time vehicle data.Because the ClientID and Topic generation rules of MQTT (based on IMEI MD5 and UserID) are cracked, the third party can subscribe to the real-time stream data of vehicles.Topic: /oversea/res/<userId>  
4) Identity Token Hijacking RiskThe project shows how the'encryToken 'generated by the login process exports the business-layer encryption key.Once the Token is leaked, the vehicle's remote control commands (such as flashing lights, whistling, and turning on the air conditioner) will be left unprotected.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
