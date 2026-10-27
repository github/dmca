While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

One or more repositories in this DMCA takedown notice has been processed in accordance with GitHub's prohibition on sharing unauthorized product licensing keys, software for generating unauthorized product licensing keys, and/or software for bypassing checks for product licensing keys.

You can learn more in [GitHub's Acceptable Use Policies](https://docs.github.com/en/github/site-policy/github-acceptable-use-policies).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am authorized to act on the copyright owner's behalf.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

No

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

I am [private], [private] of Really Nice Images Ltd. (Companies House registered number [private], registered address [private]), acting on behalf of the company as copyright holder of the works listed below. I have full authority to submit copyright complaints and to sign this notification on the company's behalf.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The infringed works are the following commercial software products of Really Nice Images Ltd.:

- RNI Films for iOS (App Store item id 1140465593, bundle identifier reallyniceimages.rni) — described at https://reallyniceimages.com/store.html
- RNI Aerochrome for iOS — described at https://reallyniceimages.com/store.html

Both applications are commercial software distributed through the Apple App Store as free downloads with in-app purchases that unlock premium features ("Films Pro" and "Aerochrome Pro"). Access to those premium features is technically protected by an entitlement check that our app makes against our licence-status API at [private], which is under our exclusive control. Only paying customers whose in-app purchase is validated receive a response granting FilmsProStatus = 1 or AeroProStatus = 1.

Really Nice Images Ltd. owns the copyright in both of these applications, in the client-side licence-verification logic they contain, and in the server responses they consume.

**If the original work referenced above is available online, please provide a URL.**

https://reallyniceimages.com/store.html

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

The following four (4) URLs contain material that infringes on the copyrighted works described above. All four are copies or derivatives of the exact bypass script that GitHub took down in RNI's DMCA notice dated 2022-10-26 (repository [private], file Crack/rf.js, notice archived at [private]).

URL 1 — a byte-identical copy of the 2022 bypass script (SHA-256 fee0f08f3361a0abc1d821ec589c36c3efe630c94347bfb8a195e6743f36aa4b) — a JavaScript file explicitly named "RNI films" in its header, purposefully designed to subvert the app protection of RNI Films for iOS and RNI Aerochrome for iOS and obtain unsolicited access to their paid filter sets:  
https://github.com/hujiejeff/Cuttlefish/blob/master/Crack/rf.js

URL 2 — a Quantumult X .conf configuration file in the same repository that binds the API hostname pro-status-service-prod.azurewebsites.net to the bypass script above, making it operative on end-user devices. Contains the line "# > 076 RNI解锁高级会员（2022-02-15）by [private]" ("RNI unlock premium membership"):
https://github.com/hujiejeff/Cuttlefish/blob/master/Rewrite/UnlockApp.conf

URL 3 — a byte-identical copy of the same bypass script (SHA-256 fee0f08f3361a0abc1d821ec589c36c3efe630c94347bfb8a195e6743f36aa4b, verified identical to URL 1), renamed RNIFilms.js. Uploaded on 11 April 2024, i.e. after GitHub took down the original in the 2022 notice, so this is a re-upload of previously-noticed infringing material:  
https://github.com/amiglistimo/Quantumult-X/blob/main/Scripts/RNIFilms.js

URL 4 — a Quantumult X .conf configuration file that binds the same API hostname to the bypass script and explicitly labels the target "# RNI Films", replicating the 2022 infringement pattern:  
https://github.com/Hoyou06/SamiyaCrack/blob/main/Rewrite/UnblockVip/VIP_NoUse.conf

The accused files are not merely discussing our product. They contain code that intercepts network traffic to our licence-verification server, rewrites the server's response to falsely indicate premium entitlement, and thereby grants users unauthorised access to premium features they have not paid for. This directly circumvents a technical measure that effectively controls access to our copyrighted work, and the code is purposefully designed to do so.

Requested remedy:

1. Prompt disabling of all four (4) URLs listed above. All four files are used together as a working infringement toolkit for RNI Films and RNI Aerochrome on iOS, so partial removal will not resolve the infringement.

2. Account-level review under GitHub's repeat-infringer policy. URL 3 (amiglistimo/Quantumult-X/Scripts/RNIFilms.js) is a byte-identical re-upload of a file GitHub already took down from [private] on 2022-10-26 pursuant to RNI's earlier notice, uploaded 18 months after that takedown. URLs 1 and 2 reproduce the same file and its loader configuration. Removal alone will not deter re-uploading; account-level enforcement under GitHub's DMCA policy §H is warranted.

3. To the extent GitHub considers the accused files under its acceptable-use policy against "software for bypassing checks for product licensing keys" (as it did in the 2022-10-26 processing), we ask that this framing also be applied here. The accused code sets FilmsProStatus and AeroProStatus fields inside a licence-verification response to falsely indicate a valid purchase.

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Access to the premium features of RNI Films for iOS and RNI Aerochrome for iOS is protected by a server-side entitlement check. On launch and at intervals, the apps call our licence-status API at [private], which is under Really Nice Images Ltd.'s exclusive control. The API returns fields including FilmsProStatus and AeroProStatus. Premium filters and features are unlocked only when the API returns FilmsProStatus = 1 or AeroProStatus = 1 for a validated in-app purchase. This response-based entitlement gate is a technical measure that effectively controls access to the copyrighted software and its premium content.

**How is the accused project designed to circumvent your technological protection measures?**

The accused JavaScript files (URLs 1 and 3) are Quantumult X / Cuttlefish "rewrite" scripts that intercept the app's HTTPS request to [private], parse the JSON response body, forcibly set FilmsProStatus = 1 and AeroProStatus = 1, and return the modified body to the app. The two .conf configuration files (URLs 2 and 4) bind the API's hostname to those bypass scripts so that Quantumult X will apply the rewrite on the end user's device. The header comments in the JS files explicitly describe the target application as "RNI films" and the intended effect as "RNI解锁高级会员" ("RNI unlock premium membership"). The code is purposefully designed to subvert the app's licence check and grant users unpaid premium access.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

The account owners appear to operate under pseudonymous GitHub handles and I have no other contact information for them:

- hujiejeff — https://github.com/hujiejeff (owner of URLs 1 and 2)
- amiglistimo — https://github.com/amiglistimo (owner of URL 3, re-uploaded on 2024-04-11 after GitHub's 2022-10-26 takedown of the same file)
- Hoyou06 — https://github.com/Hoyou06 (owner of URL 4)

Steps taken to resolve this prior to submitting:

1. We reviewed GitHub's DMCA archive (github/dmca) and confirmed that RNI's prior notices from 2019-10-24, 2022-10-26 and 2026-08-28 accurately describe the same category of infringement and were accepted by GitHub Trust & Safety.  
2. We downloaded the accused files listed above and verified that URL 1 and URL 3 are byte-identical (SHA-256 fee0f08f3361a0abc1d821ec589c36c3efe630c94347bfb8a195e6743f36aa4b) copies of the file GitHub disabled at [private] on 2022-10-26. URLs 2 and 4 are configuration files whose sole purpose is to load and execute that same bypass code against our licence-verification server.  
3. In view of the anonymous / obfuscated identities of the account holders and the fact that at least two of them are re-uploads of already-noticed material, and given that direct contact with [private]-derived accounts has historically not resulted in voluntary compliance, we consider a DMCA notice to GitHub the appropriate next step.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private], [private] Really Nice Images Ltd. [private] mail: [private] Phone: [private]

**Please type your full name for your signature.**

[private]
