Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).
- offered to connect the owners of the affected repositories with [legal resources](https://github.blog/2020-11-16-standing-up-for-developers-youtube-dl-is-back/#developer-defense-fund) as part of our commitment to standing up for developers.

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf?**  
  
Yes, I am authorized to act on the copyright owner's behalf.  
  
**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**  
  
No  
  
**Does your claim involve content on GitHub or npm.js?**  
  
GitHub  
  
**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**  
  
I am an [private] [private] the copyright owner, Future Motion, Inc. ("Future Motion"). Future Motion asked me to file this DMCA notice.  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed. If possible, include a URL to where it is posted online.**  
  
The original copyrighted work is the firmware that resides in the controller of a Future Motion Onewheel self-balancing electric skateboard (see www.onewheel.com). This firmware is confidential and therefore is not posted anywhere online. However, the nature of this complaint is circumvention of technological measures (see below), rather than copying the copyrighted firmware.  
  
**What files should be taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**  
  
• https://github.com/ffmwheel/rewheel-main  
• https://github.com/non-bin/rewheel  
• https://github.com/non-bin/rewheel/tree/Fleek  
• https://github.com/non-bin/rewheel/tree/ota-changes-branch  
• https://github.com/apopkalypse/rewheel-main  
• https://github.com/taquitos/rewheel-main  
• https://github.com/SrirachaBoi/srirachaboi.github.io  
• https://github.com/SrirachaBoi/FFMWheel  
• https://github.com/Yanndd1/rewheel  
• https://github.com/Camwp/rewheel  
• https://github.com/yourjelly/rewheel  
• https://github.com/danilo-dreia/rewheel/  
• https://github.com/ekarios/rewheel  
• https://github.com/Intelli/rewheel  
• https://github.com/MarcBelmaati/rewheel-danish  
• https://github.com/2refocus/rewheel  
• https://github.com/awbeepbeep/rewheel  
• https://github.com/bmigette/rewheel  
• https://github.com/broderick-s/hanwheel  
• https://github.com/sdmods/rewheel  
• https://github.com/c0psrul3/rewheel  
• https://github.com/crashf/rewheel  
• https://github.com/eagsalazar/rewheel  
• https://github.com/FabioBatSilva/rewheel  
• https://github.com/glashtin/rewheel  
• https://github.com/illperipherals/rewheel  
• https://github.com/jakepurple13/rewheel  
• https://github.com/jldesignseu/rewheel  
• https://github.com/joemoongit/rewheel  
• https://github.com/KyleJFischer/rewheel  
• https://github.com/MitchellHayes/rewheel  
• https://github.com/Mitchlol/rewheel  
• https://github.com/nickcica/rewheel  
• https://github.com/Relys/rewheel  
• https://github.com/rtcfirefly/rewheel  
• https://github.com/spitfire1337/rewheel  
• https://github.com/StevenMatchett/rewheel  
• https://github.com/tylerturdenpants/rewheel  
• https://github.com/WantedRobot/rewheel  
• https://github.com/WiBla/rewheel  
• https://github.com/xenorim/rewheel  
• https://github.com/yourjelly/rewheel  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
Yes  
  
**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**  
  
Future Motion uses two primary technological protection measures to prevent access to the Onewheel firmware. First, before a Onewheel board will communicate wirelessly with any software application, the application must provide a “handshake” password unique to each Onewheel board. The passwords are stored on a Future Motion web server, which is designed to be accessible only to authorized Future Motion software applications. Second, Future Motion uses model-specific secret 128-bit encryption keys that must be provided and recognized by the board processor before new Onewheel firmware can be installed on the board.  
  
**How is the accused project designed to circumvent your technological protection measures?**  
  
The ReWheel software tools Future Motion accuses of violating the DMCA operate by (a) decrypting the authorized Onewheel firmware, (b) modifying the authorized Onewheel firmware to remove safety features, (c) re-encrypting the modified, unauthorized firmware with Future Motion’s encryption key, and/or (d) “spoofing” the Onewheel firmware by uploading the unauthorized firmware version along with the encryption key, thereby causing the Onewheel processor to mistakenly recognize the unauthorized firmware as an authorized version.  
  
More specifically, with respect to the product-specific “handshake” password required for wireless communication between a software application and a Onewheel board, the ReWheel software tools wirelessly connect to a Onewheel board and either mimic or run concurrently with the authorized Onewheel software application, thereby obtaining the required password from the Future Motion server and unlocking communications between the ReWheel software and the Onewheel board.  
  
With respect to the model-specific 128-bit encryption keys that must be provided along with any new Onewheel firmware, the ReWheel repositories provide software tools and instructions for extracting the encryption key from Onewheel firmware already resident on a Onewheel board, and then transmitting the key back to the ReWheel software, where it can be used to install unauthorized firmware onto the Onewheel board.  
  
In summary, in order for the ReWheel software to function, it must extract Future Motion’s board-specific password and model-specific encryption keys, thereby circumventing Future Motion’s technological measures that effectively control access to Future Motion’s copyrighted Onewheel firmware. The ReWheel software is designed solely for this purpose.  
  
**<a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">Have you searched for any forks</a> of the allegedly infringing files or repositories? Each fork is a distinct repository and must be identified separately if you believe it is infringing and wish to have it taken down.**  
  
Yes, the forks are included in the previous list of URLs.  
  
**Is the work licensed under an open source license?**  
  
No  
  
**What would be the best solution for the alleged infringement?**  
  
Reported content must be removed  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
No.  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
[private]  
[private]  
[private]  
[private]  
  
**Please type your full legal name below to sign this request.**  
  
[private]  
