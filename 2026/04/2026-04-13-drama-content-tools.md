Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

. NATURE OF THIS NOTICE

This notice addresses violations of the anti-circumvention provisions of 17 U.S.C. § 1201(a)(2). The repositories identified herein traffic in technology that facilitates circumvention of technological protection measures implemented by authorized streaming services (including Rakuten Viki and other licensed platforms) to control access to copyrighted Korean, Chinese, Japanese, and other Asian drama content. These tools provide automated access to unauthorized copies of protected content distributed through piracy infrastructure (including [private], [private], and related clone sites, as well as their video hosting servers such as [private], [private], and [private]), thereby circumventing the subscription paywalls, digital rights management, and access controls that rights holders employ to protect their works. 

II. COMPLAINANT IDENTIFICATION

Company: Remove Your Media LLC  
Address: [private]  
Email: [private]

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

1. Providing Unauthorized Access: The tools extract streaming URLs from [private] clone sites and their associated video hosting servers ([private], [private], [private]), which host unauthorized copies of content that should only be available through licensed, paid platforms. By automating access to these pirated copies, the tools provide a circumvention pathway around the subscription paywalls of legitimate services. 

2. Bypassing DRM Through Pirate Sources: Rather than attempting to break the DRM on licensed platforms directly, these tools access content that has already been stripped of DRM protections and redistributed through piracy infrastructure. The effect is identical: users gain access to content without paying rights holders or respecting their access controls.

3. Defeating Geographic Restrictions: The piracy infrastructure these tools access has no geographic restrictions, effectively circumventing the territorial licensing controls implemented by rights holders.

4. Eliminating Authentication Requirements: These tools require no user authentication or subscription, circumventing the account-based access controls of licensed platforms.

IV. PIRACY INFRASTRUCTURE ENABLED BY THESE TOOLS 

The circumvention tools target the following piracy infrastructure:

- [private] Clone Sites ([private], [private], and others): Notorious piracy sites hosting unauthorized copies of Korean, Chinese, Japanese, and other Asian dramas licensed to streaming platforms. The original [private] network (including Asianc, Dramanice, Watchasia, Runasian, and Asianwiki) announced shutdown in November 2024 due to copyright enforcement, but clone sites continue to operate.

- [private] ([private] and clones): Long-running piracy site for Asian dramas, part of the "Kiss" network that includes KissAnime (shut down) and KissCartoon.

- [private] ([private]): Korean variety show piracy site hosting unauthorized broadcasts of shows from KBS, MBC, SBS, and other Korean networks.

- [private] ([private]): Piracy site specializing in Korean, Chinese, and Japanese dramas with subtitle support.

- [private] : Piracy aggregator for Asian drama content.

- [private] ([private]): Asian drama piracy site with streaming and download options.

- [private] ([private] and mirrors): Asian drama streaming piracy site. 

- [private] / [private] ([private], [private]): Third-party video hosting servers that store and distribute unauthorized copies of Asian drama content. These servers provide the HLS (.m3u8) streaming infrastructure that [private] and similar sites embed. 

- [private] / [private] : Additional piracy video hosting CDNs used to distribute unauthorized content accessed by these tools.

These services host content that has been pirated from licensed platforms, stripped of DRM, and made available without authorization. The GitHub tools automate and scale access to this pirated content.

 

V. CIRCUMVENTION DEVICE REPOSITORIES

 

A. Core Circumvention Library

 

Repository: [private]

Stars: 566+

Forks: 318+

Function: Node.js library implementing "providers" for numerous piracy sites including [private]. The [private] provider (src/providers/movies/dramacool.ts) extracts streaming URLs from piracy infrastructure, returning direct HLS links to unauthorized content.

Technical Evidence: Documentation at docs/providers/dramacool.md explicitly shows the library extracting m3u8 streaming URLs from piracy servers ([private], [private]) and download links from piracy domains ([private]).

§ 1201(a)(2) Analysis: This library is primarily designed for circumvention, has no commercially significant purpose other than enabling unauthorized access to copyrighted content, and is marketed for accessing content from piracy sites.

B. API Implementation

Repository: [private]

Stars: 1,500+

Forks: 700+

Function: REST API exposing the consumet.ts library's circumvention functionality via HTTP endpoints. Allows programmatic access to pirated Asian drama content through simple API calls.

Documented Endpoint: /movies/dramacool/{query} enables searching and streaming from DramaCool piracy infrastructure.

§ 1201(a)(2) Analysis: This API is primarily designed to facilitate circumvention at scale, enabling third-party applications to programmatically access pirated content without interacting with legitimate, protected platforms.

C. Kodi Media Center Addon

Repository: [private]

Stars: 43

Forks: 13

Function: Kodi addon that scrapes [private] to stream Asian dramas. Integrates piracy access directly into home media center software.

§ 1201(a)(2) Analysis: Designed solely to provide automated access to pirated content through [private] infrastructure.

D. Command Line Interface Tool

Repository: [private]

Function: CLI tool to browse and watch Korean/Chinese dramas. Repository explicitly states it "scrapes the site asianembed which is the site [private] scrapes from."
 
§ 1201(a)(2) Analysis: Directly targets the underlying piracy video hosting infrastructure that [private] relies upon.

E. Video Downloader

Repository: [private]

Function: Python application that extracts direct download URLs from [private], enabling permanent local copies of pirated content.

§ 1201(a)(2) Analysis: Facilitates circumvention by converting streaming piracy access into permanent downloads.

F. Full Piracy Streaming Sites Using Consumet

Repository: [private]

Function: Complete piracy streaming website for anime, K-dramas, movies, and manga. Environment configuration explicitly references DRAMACOOL_SCRAPER and CONSUMET_API_URL. 

Repository: [private]

Function: Another piracy streaming website explicitly using consumet API for [private] access (DOWNLOAD_API_URL references dramacool endpoint). 

§ 1201(a)(2) Analysis: These repositories demonstrate the ecosystem effect of circumvention libraries - they enable turnkey deployment of piracy streaming sites.

G. Additional Kodi Addon

Repository: [private]

Function: Alternative Kodi addon for streaming from [private] piracy infrastructure.

H. KissAsian Scrapers

Repository: https://github.com/fraxken/kissasian-video-scrapper

NPM Package: kissasian

Function: Node.js CLI tool and NPM package for scraping [private] piracy site. Extracts video embed URLs bypassing site protections including CAPTCHAs.


Repository: https://github.com/whouishere/kissasian.go

Function: Go-based [private] web scraper for educational purposes.

I. [private] Scrapers 

Repository: https://github.com/8lueberry/scraper-kshow123

Function: Kodi plugin scraper for [private] piracy site. Provides Latest, Popular, Top Rated, All shows, and Search functionality. 

J. [private] API Tools

Repository: [private]

Also: https://github.com/Dindin3515/Kisskh-API

Function: Node.js REST API for scraping [private] ([private] piracy site. Extracts media info, subtitles, and video source URLs. API endpoints mirror [private]'s internal API structure.

Technical Evidence: .env.example exposes API endpoints: API_INFO=https://[private].co/api/DramaList/Drama/ and API_EPISODE=https://[private]/api/DramaList/Episode/

K. AsianEmbed/AsianLoad API

Repository: https://github.com/parnexcodes/asianembed-api

Function: REST API scraping [private] and [private] video hosting servers that provide backend infrastructure for [private] and similar piracy sites. Built with Node.js, Express, and Cheerio.

L. Multi-Site Asian Drama Libraries

Repository: https://github.com/tbdsux/asiandrama.py

Also: https://github.com/TheBoringDude/asiandrama.py

Function: Python library for scraping multiple Asian drama piracy sites including [private] and [private]. Provides unified interface: [private] () and [private] () classes with search, fetch, and episode extraction methods.


Technical Evidence: Code examples show scraping [private] and [private] with methods like [private] (), [private] (), [private] ().

 

M. Windows Desktop Downloaders 

Repository: https://github.com/henry-richard7/Asian-Drama-Downloader

Function: Windows GUI application for downloading Asian dramas from piracy infrastructure. Uses [private] and [private] for downloading extracted streams.

Repository: https://github.com/ali-sajjad-rizavi/asian-drama-downloader

Function: Python downloader specifically targeting [private] ([private]) piracy site. Extracts and batch downloads drama episodes.


N. Mobile Applications

Repository: https://github.com/Frofter/Frofter-Asian-App

Function: Android application for streaming Asian dramas from piracy infrastructure. Companion to [private], targets [private] and [private] sources.

O. Discord Bot Integration

Repository: https://github.com/GDjkhp/NoobGPT

File: kisskh_.py

Function: Discord bot with integrated [private] scraper. Contains [private] class implementing drama search, episode listing, token generation for stream access, and subtitle extraction.

Technical Evidence: Code shows token generation bypassing [private] access controls: quickjs_context.eval(token_generation_js_code + f'_0x54b991({episode_id}...) 

VI. ABSENCE OF SUBSTANTIAL NON-INFRINGING USE

These tools have no substantial non-infringing use:

1. Target Exclusively Piracy Infrastructure: The tools are specifically designed to extract content from [private] and its video hosting servers, which host exclusively unauthorized content. There is no licensed, legitimate content on these platforms.

2. No Creative Commons or Public Domain Content: Unlike [private] (which hosts user-generated and public domain content), [private] hosts only commercially-produced dramas that are available through licensed channels such as Viki.

3. Designed to Evade Payment: The explicit purpose of these tools is to access content without using (and paying for) the licensed platforms where this content is legitimately available.

4. [private] Shutdown Acknowledges Illegality: The original [private] network's November 2024 shutdown announcement stated explicitly: "Due to the copyright, we cannot run the sites anymore." This admission confirms the piracy nature of the targeted infrastructure.

VII. DISTINCTION FROM [private]

This matter is distinguishable from the [private] takedown (October 2020) that was reversed:

- No Legitimate Content: [private] hosts substantial non-infringing content (Creative Commons, public domain, user-generated). [private] hosts exclusively pirated commercial content.

- No Legitimate Use Cases: [private] had legitimate uses (journalists, accessibility, archiving personal uploads). These tools exist solely to access pirated Asian dramas.

- Targets Piracy Infrastructure: [private] accessed a legitimate platform. These tools specifically target sites that themselves shut down citing copyright enforcement.

- Rights Holder Enforcement: The [private] government's Ministry of Culture, Sports and Tourism, in partnership with Interpol's Stop Online Piracy (I-SOP) Project, actively targets sites like [private] for copyright enforcement.

VIII. REQUESTED ACTION

We request that GitHub:

- Remove [private] (or at minimum the [private] provider)

- Remove [private] (or at minimum the [private] endpoints)

- Remove [private] and forks

- Remove [private]

- Remove [private]

- Remove [private]

- Remove [private]

- Remove [private]

- Remove fraxken/kissasian-video-scrapper

- Remove whouishere/kissasian.go

- Remove 8lueberry/scraper-kshow123

- Remove Inside4ndroid/Unofficial-Kisskh-API and Dindin3515/Kisskh-API

- Remove parnexcodes/asianembed-api

- Remove tbdsux/asiandrama.py

- Remove henry-richard7/Asian-Drama-Downloader

- Remove ali-sajjad-rizavi/asian-drama-downloader

- Remove Frofter/Frofter-Asian-App

- Remove GDjkhp/NoobGPT (or kisskh_.py file)

- Remove additional repositories and forks containing circumvention code for [private], [private], [private], [private], [private], [private], and related piracy sites 

IX. REQUIRED DECLARATIONS

I declare under penalty of perjury that: 

- I am authorized to act on behalf of the copyright owners whose works are made available through licensed platforms protected by access controls that these tools circumvent.

- I have a good faith belief that the repositories identified herein traffic in circumvention devices in violation of 17 U.S.C. § 1201(a)(2).

- The information in this notice is accurate.

- I understand that abuse of this process may result in liability under 17 U.S.C. § 512(f).


Respectfully submitted,
 

/s/ [private]  
[private]  
Remove Your Media LLC  
[private]   
[private]   
