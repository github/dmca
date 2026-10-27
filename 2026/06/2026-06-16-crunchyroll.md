Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

1. SUBMITTING PARTY / AUTHORIZED AGENT

This notice is submitted by Remove Your Media LLC, the authorized anti-piracy enforcement agent for Crunchyroll, LLC ("Crunchyroll").

  Agent:   [private], Authorized Agent

  Company: Remove Your Media LLC

  Address: [private]

  Email:  [private]

I am authorized to act on behalf of Crunchyroll, the owner and exclusive licensee of rights in the copyrighted works whose access controls are circumvented by the technology identified below.


2. STATEMENT OF THE CLAIM

 

This notice is submitted pursuant to the Digital Millennium Copyright Act, 17 U.S.C. Section 1201, which prohibits trafficking in technology, products, services, or components that are primarily designed or produced to circumvent technological protection measures ("TPMs") controlling access to copyrighted works (Section 1201(a)(2)) and protecting the rights of a copyright owner (Section 1201(b)(1)).

The repository identified below distributes source code and a built application that is primarily designed to circumvent the technological measures Crunchyroll uses to control access to, and copying of, its licensed catalog.

3. REPOSITORY IDENTIFIED

Primary repository:

https://github.com/Crunchy-DL/Crunchy-Downloader

GitHub is also requested to process this notice against forks within the repository network. Based on the representative number of forks I have reviewed, I believe that all or most of the forks are infringing to the same extent as the parent repository. 

4. THE COPYRIGHTED WORKS PROTECTED BY THE TECHNOLOGICAL MEASURES

Crunchyroll holds exclusive distribution and reproduction rights, by license from rights holders, in a large catalog of copyrighted audiovisual works (anime series and films) and associated copyrighted musical works, made available to authenticated, paying subscribers through Crunchyroll's streaming service at www.crunchyroll.com and store.crunchyroll.com. These works are protected by U.S. copyright (17 U.S.C. Section 106), and Crunchyroll's exclusive rights are
enforceable under 17 U.S.C. Section 501. Representative titles affected include the full slate of anime series and films in the Crunchyroll catalog, together with the soundtrack and music content delivered through the same service.

5. THE TECHNOLOGICAL PROTECTION MEASURES AT ISSUE

Crunchyroll's streaming service employs technological measures that effectively control access to, and copying of, the copyrighted works above:

  (1) Widevine DRM / encrypted streams. Crunchyroll delivers its catalog

  as encrypted media streams protected by Widevine Content Decryption

  Module (CDM) digital rights management. In the ordinary course of

  operation, a licensed Widevine client negotiates a decryption license

  with Crunchyroll's license server and decrypts content transiently, in

  memory, solely for authorized playback. Decryption keys are not exposed

  to the end user, and the protected work cannot be accessed except

  through this authorized, authenticated pathway. This measure effectively

  controls access to the works within the meaning of Section 1201(a) and

  protects Crunchyroll's reproduction rights within the meaning of

  Section 1201(b). 

  (2) Authenticated, account-bound access. Access to the protected streams

  requires authentication through a valid Crunchyroll subscriber account.

  Access is personal, non-transferable, and limited to authorized

  playback.


  (3) Territorial / geographic access controls. Crunchyroll licenses

  content on a territory-by-territory basis and applies geographic access

  controls (IP-based geo-filtering) so that a given title is accessible

  only to authenticated users in licensed territories. 

6. HOW THE ACCUSED PROJECT IS DESIGNED TO CIRCUMVENT THOSE MEASURES

Based on the project's own publicly available documentation and source code, the Crunchy-Downloader project is designed to circumvent each of the measures described above:

  (1) Circumvention of Widevine DRM. The project's wiki instructs users to

  obtain and install Widevine CDM files described as "required for

  decryption." The application's code path retrieves content decryption

  keys and invokes external decryption utilities (e.g., mp4decrypt /

  Shaka Packager) to convert Crunchyroll's encrypted, access-controlled

  streams into decrypted media. This is not authorized, transient,

  in-memory playback - it is the deliberate defeat of the Widevine access

  control so that protected content can be obtained in the clear. This

  conduct falls squarely within Section 1201(a)(2) (trafficking in

  access-circumvention technology) and Section 1201(b)(1) (trafficking in

  technology that circumvents copy-protection measures).

  (2) Production of permanent, unprotected copies. After decryption, the

  application muxes the output into permanent, DRM-free media files (MP4,

  MKV, MP3). This converts a personal, non-transferable, access-limited

  stream into portable files that can be reproduced and redistributed

  without limitation, materially increasing the risk of unauthorized

  distribution beyond the personal-access model of the licensed service.

  The project documents MP3 output for music content (see

  CRD/Downloader/Crunchyroll/CrunchyrollManager.cs), confirming that the

  tool targets both anime and music catalog content.


  (3) Circumvention of territorial access controls. The project documents

  proxy support and IP-check functionality. In combination with

  Crunchyroll's territory-based licensing and geo-filtering, this

  functionality is designed to defeat the geographic access controls and

  access content from territories where the user is not authorized.


  (4) Disclaimers do not cure the violation. The wiki includes "private

  use only" language and acknowledges potential Terms of Service and legal

  exposure. A disclaimer does not neutralize liability under Section 1201

  where the tool's primary design and practical use is to circumvent

  access controls and enable unauthorized copying. The presence of these

  acknowledgments alongside explicit decryption setup instructions and

  decryption/mux code paths is itself probative of the project's

  circumventing design and purpose.


7. EVIDENTIARY REFERENCES

The following publicly available materials within the repository and its wiki document the circumventing design described above:

  - Wiki - home / disclaimer / getting-started (Widevine CDM "required for decryption" setup steps):

    https://github.com/Crunchy-DL/Crunchy-Downloader/wiki

  - Wiki - proxy settings (proxy / IP-check functionality):

    https://github.com/Crunchy-DL/Crunchy-Downloader/wiki/5-%E2%80%90-General-Settings#proxy-settings
    
  - Source - MP3 output for music content:

    https://github.com/Crunchy-DL/Crunchy-Downloader/blob/65200147a0487c11874e9489c5ffbffc2bfda28d/CRD/Downloader/Crunchyroll/CrunchyrollManager.cs#L634

8. APPLICABLE LAW 

  - 17 U.S.C. Section 1201 - prohibition on circumvention of access controls and on trafficking in circumvention technology

  - 17 U.S.C. Section 1203 - civil remedies, including injunctive relief, impoundment, and damages

  - 17 U.S.C. Section 1204 - criminal penalties for willful violations committed for commercial advantage or private financial gain

  - 17 U.S.C. Sections 106, 501 - exclusive rights of the copyright owner and the framework for infringement

  - 17 U.S.C. Section 506; 18 U.S.C. Section 2319 - criminal copyright infringement thresholds and penalties

9. REQUESTED ACTION

Crunchyroll, through its authorized agent, requests that GitHub:

  (1) Disable or remove access to the repository identified in Section 3, and to its forks, for violating 17 U.S.C. Section 1201;

  (2) Notify the affected repository owner(s) that their repositories have been disabled for this reason;

  (3) Preserve all records related to these repositories for potential litigation purposes; and

  (4) Take appropriate action under GitHub's repeat-infringer policy with respect to users who repeatedly host circumvention tools.


10. REQUIRED STATEMENTS

 

I have a good faith belief that the use of the technological protection measures described above is not authorized by Crunchyroll, its agent, or the law, and that the repository identified in this notice is primarily designed to circumvent those measures in violation of 17 U.S.C.

Section 1201.

The information in this notification is accurate, and under penalty ofmperjury, I am authorized to act on behalf of Crunchyroll, LLC, the owner and exclusive licensee of rights that are protected by the technological measures circumvented by the technology identified herein.

I have read and understand GitHub's Guide to Filing a DMCA Notice.


11. SIGNATURE

/s/ [private]  
[private]  
Authorized Agent, Remove Your Media LLC  
On behalf of Crunchyroll, LLC  
[private]  
[private]
