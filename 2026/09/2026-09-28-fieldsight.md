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
  
The work is "FieldSight — Agricultural Weather Observer", a single-page web application (HTML + JavaScript) that aggregates official meteorological data and presents agricultural weather analysis across eight commodity groups. I am its [private] [private].  
  
Principal files and their SHA-256 hashes as of 10 August 2026:   
- fieldsight.html — 102,225 bytes, 1,635 lines — f57c86e8bad616eb6e9baf87c302cca2bf36db2be841d559f75814e2686f3cc7  
- fieldsight-data.js — 90,886 bytes, 862 lines — 87f626ac981ca231cfc5bbc6d6a9b1906838b2ee3002f5e22851df2355b28758  
- index.html — 53,142 bytes, 1,012 lines — 0df9a6b7331e65984730fc4179150091e1773cf68aae64c22ae597cc3c245c79  
- data.js — 33,278 bytes, 412 lines — 0bf15da43033b7a37a32995f30bcc5c3993b656a5e51904353a7e967e0844689  
  
DISTINCTIVE ELEMENTS OF ORIGINAL AUTHORSHIP  
  
Each of the following is an arbitrary design choice for which many functionally equivalent alternatives existed. Correspondence in such arbitrary elements is probative of copying rather than independent creation.  
  
1. Section architecture: eleven sections labelled with Chinese ordinal numerals 壹 through 拾壹 (Climate Overview, Ocean Indices, China Forecast Charts, U.S. Forecast Atlas, Special Weather Events, Production-Region Monitoring, Growth Stages and Sensitive Windows, Strategy Observation, News Digest, Typhoon Monitoring, Extreme Weather Impact Projection).  
  
2. An undocumented URL-derivation algorithm for China Meteorological Administration imagery, which [private] derived by systematic comparison of sample URLs: {date}{run-hour HHmm}{lead-time FFF} plus a two-digit constant, seventeen digits in total, applied across the product codes SEVP_NMC_STFC_SFER_ER24, SEVP_NMC_RFFC_SNWFD_ETM, SEVP_NMC_STFC_SFER_ET0, SEVP_NMC_AMSM_CAGMSS_ESRH, SEVP_NMC_AMDF_SFER_EDRF and SEVP_NMC_CSPB_SFER_EME. This pattern appears in no official documentation.  
  
3. Custom risk-threshold constants of [private] own devising: DROUGHT_RELIEF_CLEAR = 25, DROUGHT_RELIEF_EASE = 10, SPECIAL_WEATHER_MM = 50, HEAT_AGGRAVATE_DAYS = 8, NEWS_WINDOW_HOURS = 12.  
  
4. A network of 172 monitoring stations across eight commodities in a specific two-level hierarchy (palm oil 3 groups/6 stations; cotton 14/25; corn 11/29; soybeans 7/23; wheat 8/20; coffee 4/11; sugar 9/22; canola 12/36).  
  
5. A source-verifiability scheme using paired SOURCE_CHECKED / STATIC_UPDATED fields and a six-element provenance tuple attached to every data item.  
  
6. A news-expiry mechanism filtering ISO 8601 timestamps against NEWS_WINDOW_HOURS, hiding expired items and displaying an explicit empty state.  
  
EVIDENCE OF COPYING  
  
(a) Verbatim source code. The file data.js in the infringing repository reproduces [private] source code verbatim, including elements that could not arise independently. It contains [private] World Ag Weather run-ID probe configuration with [private] exact empirically-derived anchor constants — anchorId: 3121, anchorDate: '2026-07-02', perDay: 4, margin: 6, maxProbe: 60. It reproduces [private] monitoring-station coordinates and growing-season start dates identically (for example 崇左 22.38/107.36 gddStart '2026-03-01'; 哈尔滨 45.75/126.63 gddStart '2026-05-01'), and [private] Chinese-language source comments verbatim, including "gddStart=生长季起始日(null=多年生/非生长季, 用滚动37天)". The party has substituted their own news entries while retaining [private] structure, comments and configuration data.  
  
(b) Admission in their own README. The infringing repository's README states: "本地复刻版本：已同步至远程最新版（2026-08-10）" — that it is a local replica synced to a remote latest version as of 10 August 2026.  
  
(c) An impossible description inherited from an earlier version of [private] work. Their README describes the work as "8大品种 107个产区" (eight commodities, 107 production regions). That combination never existed in [private] work: [private] project contained 107 monitoring stations when it covered seven commodities, and 172 stations by the time it covered eight. This internal inconsistency indicates copying of [private] earlier descriptive text rather than independent authorship.  
  
(d) Reproduction of material created the same day. The infringing page reproduces [private] section 拾壹 ("极端天气 → 中国农产品影响推演") together with its subtitle "未来10-14天 · 官方预报为据 · 影响判断为本站条件性推断". [private] authored that section on 10 August 2026. Their README still describes the page as having "10 个板块" (ten sections) while the live page renders eleven, indicating an incomplete sync of copied material.  
  
(e) Verbatim interface text. The infringing page reproduces [private] subtitle for section 叁 ("中央气象台 降水预报1-7天 · 土壤墒情10-50cm · 最高气温预报1-7天 · 逐时气温实况 · 强对流/雷达/卫星 · 时间戳自动探测") and [private] page footer, including the distinctive phrases "双源容灾" and "积温自各产区生长季起始日累计".  
  
The only substantive alterations are the replacement of [private] attribution with the branding "建发农产品" and the addition of that party's logo.  
  
SCOPE OF THIS CLAIM  
  
I am not asserting any claim over the other files in that repository (fetch_data.py, the CSV data files, scripts/, docs/, assets/, memo8.10.md), which appear to be that party's own material. My claim is limited to the three files identified below.  
  
The repository containing [private] work includes no LICENSE file. All rights are reserved.  
  
I request that [private] [private] [private] [private] be redacted from any public publication of this notice.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
The work is "FieldSight — Agricultural Weather Observer", a single-page web application (HTML + JavaScript) that aggregates official meteorological data and presents agricultural weather analysis across eight commodity groups. I am its [private] [private].  
  
Principal files and their SHA-256 hashes as of 10 August 2026:  
- fieldsight.html — 102,225 bytes, 1,635 lines — f57c86e8bad616eb6e9baf87c302cca2bf36db2be841d559f75814e2686f3cc7  
- fieldsight-data.js — 90,886 bytes, 862 lines — 87f626ac981ca231cfc5bbc6d6a9b1906838b2ee3002f5e22851df2355b28758  
- index.html — 53,142 bytes, 1,012 lines — 0df9a6b7331e65984730fc4179150091e1773cf68aae64c22ae597cc3c245c79  
- data.js — 33,278 bytes, 412 lines — 0bf15da43033b7a37a32995f30bcc5c3993b656a5e51904353a7e967e0844689  
  
The work contains the following distinctive elements of original authorship. Each is an arbitrary design choice for which many functionally equivalent alternatives existed, so correspondence in these elements is probative of copying rather than independent creation:  
  
1. Section architecture: eleven sections labelled with Chinese ordinal numerals 壹 through 拾壹 (Climate Overview, Ocean Indices, China Forecast Charts, U.S. Forecast Atlas, Special Weather Events, Production-Region Monitoring, Growth Stages and Sensitive Windows, Strategy Observation, News Digest, Typhoon Monitoring, Extreme Weather Impact Projection).  
  
2. An undocumented URL-derivation algorithm for China Meteorological Administration imagery, which I derived by systematic comparison of sample URLs: {date}{run-hour HHmm}{lead-time FFF} plus a two-digit constant, seventeen digits in total, applied across the product codes SEVP_NMC_STFC_SFER_ER24, SEVP_NMC_RFFC_SNWFD_ETM, SEVP_NMC_STFC_SFER_ET0, SEVP_NMC_AMSM_CAGMSS_ESRH, SEVP_NMC_AMDF_SFER_EDRF and SEVP_NMC_CSPB_SFER_EME. This pattern appears in no official documentation.  
  
3. Custom risk-threshold constants of [private] own devising: DROUGHT_RELIEF_CLEAR = 25, DROUGHT_RELIEF_EASE = 10, SPECIAL_WEATHER_MM = 50, HEAT_AGGRAVATE_DAYS = 8, NEWS_WINDOW_HOURS = 12.  
  
4. A network of 172 monitoring stations across eight commodities in a specific two-level hierarchy (palm oil 3 groups/6 stations; cotton 14/25; corn 11/29; soybeans 7/23; wheat 8/20; coffee 4/11; sugar 9/22; canola 12/36).  
  
5. A source-verifiability scheme using paired SOURCE_CHECKED / STATIC_UPDATED fields and a six-element provenance tuple attached to every data item.  
  
6. A news-expiry mechanism filtering ISO 8601 timestamps against NEWS_WINDOW_HOURS, hiding expired items and displaying an explicit empty state.  
  
The repository containing [private] work includes no LICENSE file. All rights are reserved.  
  
EVIDENCE OF COPYING:  
  
The infringing repository's own README states: "本地复刻版本：已同步至远程最新版（2026-08-10）" — that it is a local replica synced to a remote latest version as of 10 August 2026.  
  
That README also describes the work as "8大品种 107个产区" (eight commodities, 107 production regions), a combination that never existed in [private] work: [private] project contained 107 monitoring stations when it covered seven commodities, and 172 stations by the time it covered eight. This internally inconsistent description indicates copying of an earlier version of [private] descriptive text rather than independent creation.  
  
The infringing page reproduces [private] section 拾壹 ("极端天气 → 中国农产品影响推演") together with its subtitle "未来10-14天 · 官方预报为据 · 影响判断为本站条件性推断". I authored that section on 10 August 2026. The infringing README still describes the page as having "10 个板块" (ten sections) while the live page renders eleven, indicating an incomplete sync of copied material.  
  
The infringing page also reproduces verbatim [private] subtitle for section 叁 ("中央气象台 降水预报1-7天 · 土壤墒情10-50cm · 最高气温预报1-7天 · 逐时气温实况 · 强对流/雷达/卫星 · 时间戳自动探测") and [private] page footer, including the distinctive phrases "双源容灾" and "积温自各产区生长季起始日累计".  
  
The only substantive alterations are the replacement of [private] attribution with the branding "建发农产品" and the addition of that party's logo.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://github.com/CACJNJ/weather-panel  
[private]
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
Specific files within the repository are infringing  
  
**Identify only the specific file URLs within the repository that is infringing:**  
  
https://github.com/CrazyLinzo/weather-panel/blob/master/fieldsight.html  
https://github.com/CrazyLinzo/weather-panel/blob/master/fieldsight-data.js  
https://github.com/CrazyLinzo/weather-panel/blob/master/data.js  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
**Is the work licensed under an open source license?**  
  
No  
  
**What would be the best solution for the alleged infringement?**  
  
Reported content must be removed  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
GitHub username: CrazyLinzo (https://github.com/CrazyLinzo)  
GitHub user ID: [private]    
Repository: CrazyLinzo/weather-panel (repository ID [private])  
Published site: [private]  
The site is branded "[private] / [private]" and displays that party's logo. I do not have a direct email address for this party.  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]
