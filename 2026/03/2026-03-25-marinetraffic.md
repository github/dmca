Before disabling any content in relation to this takedown notice, GitHub  
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).  
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).  
  
To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).  
  
---  
  
While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).  
  
---  
  
**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**  
  
Yes, I am authorized to act on the copyright owner's behalf.  
  
**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**  
  
No  
  
**Does your claim involve content on GitHub or npm.js?**  
  
GitHub  
  
**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**  
  
I'm [private] of [private] at Kpler Holding SA, the owner of the MarineTraffic platform (http://www.marinetraffic.com) and all associated intellectual property, data, and content. I am authorized to act on behalf of Kpler in matters of intellectual property enforcement.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
MarineTraffic (http://www.marinetraffic.com) is a proprietary maritime intelligence platform owned and operated by Kpler Holding SA. The platform provides real-time and historical vessel tracking data, ship positions, and maritime analytics derived from AIS (Automatic Identification System) data. This includes proprietary map visualizations, vessel position data, ship type classifications, and movement status indicators displayed on the platform's interactive map interface.  
The copyrighted works include the platform's proprietary data, map tile renderings, vessel marker visualizations (including color-coded ship type and motion state indicators), and the structured data derived from Kpler's processing and aggregation of raw AIS signals. All data, content, and visualizations on MarineTraffic are protected under Kpler's Terms of Use (https://www.marinetraffic.com/en/p/terms), which explicitly prohibit web scraping, reverse engineering, and the creation of derived data from the platform.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://www.marinetraffic.com  
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
The entire repository is infringing  
  
**Identify the full repository URL that is infringing:**  
  
https://github.com/theSchaefer/OceanScrape  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
Yes  
  
**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**  
  
MarineTraffic employs Cloudflare anti-bot and DDoS protection to detect and block automated access to the platform. This includes browser verification challenges, rate limiting, and bot detection mechanisms. Additionally, the platform requires user authentication for access to certain data tiers and enforces Terms of Use that explicitly prohibit web scraping and automated data extraction. These measures effectively control access by preventing unauthorized automated tools from accessing the platform's copyrighted data and content.  
  
**How is the accused project designed to circumvent your technological protection measures?**  
  
The repository uses Patchright, an undetected fork of the Playwright browser automation framework, specifically designed to evade bot detection. It employs Cloudflare bypass techniques by navigating to MarineTraffic only once per browser session, then using JavaScript calls (Leaflet setView()) to pan across regions without triggering additional Cloudflare challenges. It rotates through proxy servers (Decodo) and spoofs browser fingerprints — including timezone, locale, geolocation, and User-Agent headers — all derived from the proxy IP's geographic location to appear as a legitimate user. It operates with multiple parallel browser tabs sharing a single authenticated session to maximize data extraction while minimizing detection. These techniques are explicitly designed to circumvent MarineTraffic's Cloudflare protection and bot detection mechanisms  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
https://github.com/gz475/OceanScrape  
https://github.com/Disorden456/OceanScrape  
https://github.com/Gaedub/OceanScrape  
https://github.com/DigiSouro/OceanScrape  
  
**Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository.**  
  
**Is the work licensed under an open source license?**  
  
No  
  
**What would be the best solution for the alleged infringement?**  
  
Reported content must be removed  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
[private] profile: [private]  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]  
