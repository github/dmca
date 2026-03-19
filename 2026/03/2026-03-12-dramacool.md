While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

I. NATURE OF THIS NOTICE

This notice addresses violations of the anti-circumvention provisions of 17 U.S.C. § 1201(a)(2). The repositories identified herein traffic in technology that facilitates circumvention of technological protection measures implemented by authorized streaming services (including Rakuten Viki and other licensed platforms) to control access to copyrighted Korean, Chinese, Japanese, and other Asian drama content. These tools provide automated access to unauthorized copies of protected content distributed through piracy infrastructure (including [private], [private], and related clone sites, as well as their video hosting servers such as [private], [private], and d[private]), thereby circumventing the subscription paywalls, digital rights management, and access controls that rights holders employ to protect their works.


II. COMPLAINANT IDENTIFICATION

Company: Remove Your Media LLC  
Address: [private]  
Email: [private]  
Phone: [private]

Role: Authorized representative of copyright holders whose Korean drama, Chinese drama, Japanese drama, and other Asian entertainment content is distributed through licensed platforms protected by technological access controls.

III. TECHNOLOGICAL PROTECTION MEASURES OF RIGHTS HOLDERS

This section describes the access controls implemented by our clients that these tools effectively circumvent. 

A. Access Controls Implemented by Licensed Platforms

Our clients control access to their copyrighted Asian drama content through multiple technological protection measures:

1. Subscription Paywalls: Platforms such as Rakuten Viki require paid subscriptions to access their full catalogs of licensed Asian drama content. 

2. Digital Rights Management (DRM): Licensed platforms employ DRM technologies to prevent unauthorized copying and redistribution of video streams. 

3. Geographic Access Controls: Content is licensed on a territorial basis, with geo-blocking ensuring content is only accessible in regions where distribution rights have been secured.

4. Authentication Systems: User account authentication ensures only authorized subscribers can access premium content.


B. How These Tools Circumvent Rights Holders' Access Controls

The repositories identified in this notice circumvent these access controls by:

1. Providing Unauthorized Access: The tools extract streaming URLs from DramaCool clone sites and their associated video hosting servers ([private], [private]), which host unauthorized copies of content that should only be available through licensed, paid platforms. By automating access to these pirated copies, the tools provide a circumvention pathway around the subscription paywalls of legitimate services.

2. Bypassing DRM Through Pirate Sources: Rather than attempting to break the DRM on licensed platforms directly, these tools access content that has already been stripped of DRM protections and redistributed through piracy infrastructure. The effect is identical: users gain access to content without paying rights holders or respecting their access controls.

3. Defeating Geographic Restrictions: The piracy infrastructure these tools access has no geographic restrictions, effectively circumventing the territorial licensing controls implemented by rights holders.

4. Eliminating Authentication Requirements: These tools require no user authentication or subscription, circumventing the account-based access controls of licensed platforms.

IV. PIRACY INFRASTRUCTURE ENABLED BY THESE TOOLS

The circumvention tools target the following piracy infrastructure:

- DramaCool Clone Sites ([private], [private], and others): Notorious piracy sites hosting unauthorized copies of Korean, Chinese, Japanese, and other Asian dramas licensed to streaming platforms. The original DramaCool network (including [private], [private], [private], [private], and [private]) announced shutdown in November 2024 due to copyright enforcement, but clone sites continue to operate. 

- [private] / [private] : Third-party video hosting servers that store and distribute unauthorized copies of Asian drama content. These servers provide the HLS (.m3u8) streaming infrastructure that DramaCool embeds.

- [private] / [private] : Additional piracy video hosting CDNs used to distribute unauthorized content accessed by these tools. 

These services host content that has been pirated from licensed platforms, stripped of DRM, and made available without authorization. The GitHub tools automate and scale access to this pirated content. 

V. CIRCUMVENTION DEVICE REPOSITORIES

A. Core Circumvention Library

Repository: https://github.com/consumet/consumet.ts  
Stars: 566+  
Forks: 318+

Function: Node.js library implementing "providers" for numerous piracy sites including DramaCool. The DramaCool provider (src/providers/movies/dramacool.ts) extracts streaming URLs from piracy infrastructure, returning direct HLS links to unauthorized content.

Technical Evidence: Documentation at docs/providers/dramacool.md explicitly shows the library extracting m3u8 streaming URLs from piracy servers ([private], [private]) and download links from piracy domains ([private]).

§ 1201(a)(2) Analysis: This library is primarily designed for circumvention, has no commercially significant purpose other than enabling unauthorized access to copyrighted content, and is marketed for accessing content from piracy sites.

B. API Implementation

Repository: https://github.com/consumet/api.consumet.org  
Stars: 1,500+  
Forks: 700+

Function: REST API exposing the consumet.ts library's circumvention functionality via HTTP endpoints. Allows programmatic access to pirated Asian drama content through simple API calls.

Documented Endpoint: /movies/dramacool/{query} enables searching and streaming from DramaCool piracy infrastructure.

§ 1201(a)(2) Analysis: This API is primarily designed to facilitate circumvention at scale, enabling third-party applications to programmatically access pirated content without interacting with legitimate, protected platforms.

C. Kodi Media Center Addon

Repository: https://github.com/groggyegg/plugin.video.dramacool  
Stars: 43  
Forks: 13

Function: Kodi addon that scrapes DramaCool to stream Asian dramas. Integrates piracy access directly into home media center software.

§ 1201(a)(2) Analysis: Designed solely to provide automated access to pirated content through DramaCool infrastructure.

D. Command Line Interface Tool

Repository: https://github.com/CoolnsX/dra-cla

Function: CLI tool to browse and watch Korean/Chinese dramas. Repository explicitly states it "scrapes the site asianembed which is the site dramacool scrapes from."

§ 1201(a)(2) Analysis: Directly targets the underlying piracy video hosting infrastructure that DramaCool relies upon.

E. Video Downloader

Repository: https://github.com/hyndex/Dramacool-video-downloader

Function: Python application that extracts direct download URLs from DramaCool, enabling permanent local copies of pirated content.

§ 1201(a)(2) Analysis: Facilitates circumvention by converting streaming piracy access into permanent downloads. 

F. Full Piracy Streaming Sites Using Consumet

Repository: https://github.com/real-zephex/Dramalama

Function: Complete piracy streaming website for anime, K-dramas, movies, and manga. Environment configuration explicitly references DRAMACOOL_SCRAPER and CONSUMET_API_URL.

Repository: https://github.com/avalynndev/enjoytown

Function: Another piracy streaming website explicitly using consumet API for DramaCool access (DOWNLOAD_API_URL references dramacool endpoint).

§ 1201(a)(2) Analysis: These repositories demonstrate the ecosystem effect of circumvention libraries - they enable turnkey deployment of piracy streaming sites.

G. Additional Kodi Addon

Repository: https://github.com/henry-richard7/Kodi-Dramacool-Henz-Addon

Function: Alternative Kodi addon for streaming from DramaCool piracy infrastructure.

VI. ABSENCE OF SUBSTANTIAL NON-INFRINGING USE
  
These tools have no substantial non-infringing use: 

1. Target Exclusively Piracy Infrastructure: The tools are specifically designed to extract content from DramaCool and its video hosting servers, which host exclusively unauthorized content. There is no licensed, legitimate content on these platforms.

2. No Creative Commons or Public Domain Content: Unlike [private] (which hosts user-generated and public domain content), DramaCool hosts only commercially-produced dramas that are available through licensed channels such as [private], [private], and [private].

3. Designed to Evade Payment: The explicit purpose of these tools is to access content without using (and paying for) the licensed platforms where this content is legitimately available. 

4. DramaCool Shutdown Acknowledges Illegality: The original DramaCool network's November 2024 shutdown announcement stated explicitly: "Due to the copyright, we cannot run the sites anymore." This admission confirms the piracy nature of the targeted infrastructure. 

VII. DISTINCTION FROM [private]

This matter is distinguishable from the [private] takedown (October 2020) that was reversed:

- No Legitimate Content: [private] hosts substantial non-infringing content (Creative Commons, public domain, user-generated). DramaCool hosts exclusively pirated commercial content. 

- No Legitimate Use Cases: [private] had legitimate uses (journalists, accessibility, archiving personal uploads). These tools exist solely to access pirated Asian dramas. 

- Targets Piracy Infrastructure: [private] accessed a legitimate platform. These tools specifically target sites that themselves shut down citing copyright enforcement 

- Rights Holder Enforcement: The [private] government's Ministry of Culture, Sports and Tourism, in partnership with Interpol's Stop Online Piracy (I-SOP) Project, actively targets sites like DramaCool for copyright enforcement. 

VIII. REQUESTED ACTION

We request that GitHub:

- Remove consumet/consumet.ts (or at minimum the DramaCool provider)  
- Remove consumet/api.consumet.org (or at minimum the DramaCool endpoints)  
- Remove groggyegg/plugin.video.dramacool and forks  
- Remove CoolnsX/dra-cla  
- Remove hyndex/Dramacool-video-downloader  
- Remove real-zephex/Dramalama  
- Remove avalynndev/enjoytown  
- Remove henry-richard7/Kodi-Dramacool-Henz-Addon  
- Remove additional repositories and forks containing DramaCool circumvention code 

IX. REQUIRED DECLARATIONS 

I declare under penalty of perjury that:

- I am authorized to act on behalf of the copyright owners whose works are made available through licensed platforms protected by access controls that these tools circumvent.

- I have a good faith belief that the repositories identified herein traffic in circumvention devices in violation of 17 U.S.C. § 1201(a)(2). 

- The information in this notice is accurate.
  
- I understand that abuse of this process may result in liability under 17 U.S.C. § 512(f).

Respectfully submitted,  
[private]  
[private]  
Company: Remove Your Media LLC  

