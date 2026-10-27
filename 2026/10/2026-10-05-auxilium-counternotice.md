**Are you the owner of the content that has been disabled, or authorized to act on the owner’s behalf?**

Yes, I am the content owner.

**Please describe the nature of your content ownership or authorization to act on the owner's behalf.**

**What files were taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**

https://github.com/leissler/GC2_FEEL_Integration

**Do you want to make changes to your repository or do you want to dispute the notice?**

Dispute the notice.

**Please explain why you believe the material was removed as a result of a mistake or misidentification.**

I am submitting this DMCA counter notice concerning the disabled repository:  
https://github.com/leissler/GC2_FEEL_Integration

The takedown notice alleges that my repository copied the complainant's proprietary Unity Asset Store product, including script architecture, abstract class designs, metadata strings, MMF Player event handling, and camera shake logic. That allegation is mistaken.

My repository is an independently created open-source Unity integration package for interoperability between More Mountains Feel and Game Creator 2. I did not have access to the complainant's source code when I created my repository, did not copy the complainant's source code, and did not include the complainant's proprietary source code, artwork, documentation, binary assets, or other protected expression in my repository. I first accessed/downloaded the complainant's package only after the takedown, for the purpose of understanding and investigating the allegation.

The code in my repository was created by me with AI assistance and my own review, based on the behavior I wanted to implement and the documented/installed scripting APIs exposed by Feel and Game Creator 2. I did not provide the complainant's closed-source code to any AI tool.

After comparing the two packages, I do not find copied source code. There are no identical C# source files and no identical files across the packages. In the expected counterpart C# files, I found no long copied code blocks; the longest common contiguous non-comment block is only a few lines of ordinary C# or Unity/Game Creator boilerplate.

The overlap identified by the complainant consists primarily of functional concepts and short UI labels required or naturally suggested by the integration target, such as "Play MMF Player", "Pause MMF Player", "On MMF Player Play", "Wait To Complete", and similar names for Feel/Game Creator operations. These are short descriptive labels for public API functionality, not copied protected implementation.

The implementation architecture is materially different. For example, the complainant's event code attaches listeners directly to MMF Player UnityEvents, while my repository listens to Feel's MMFeedbacksEvent stream and filters the source player and event type. The complainant's camera shake feedback targets the Game Creator main camera shortcut directly, while my implementation uses configurable Game Creator camera targeting, argument context handling, diagnostics, intensity scaling, and optional stop behavior. My repository also contains substantial functionality that is not present in the complainant's product, including a custom MMF Player GameObject property type, Game Creator Actions/Conditions/Trigger feedbacks, camera viewport pulse, shot recoil, shot zoom pulse, editor validation tooling, UPM package metadata, samples, README, and MIT license.

The README statement that my project is an open-source alternative to the paid Unity Asset Store package was a factual compatibility/positioning statement, not an admission of copying. It identifies the interoperability category and the existence of a commercial product in the same category. It does not mean that I copied protected source code or other protected expression.

For these reasons, I believe the takedown notice misidentified independently created interoperability code as infringing material. I request that GitHub restore the disabled repository.

**I swear, under penalty of perjury, that I have a good-faith belief that the material was removed or disabled as a result of a mistake or misidentification of the material to be removed or disabled.**

**I consent to the jurisdiction of Federal District Court for the judicial district in which my address is located (if in the United States, otherwise the Northern District of California where GitHub is located), and I will accept service of process from the person who provided the DMCA notification or an agent of such person.**

**Please confirm that you have you have read our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-counter-notice">Guide to Submitting a DMCA Counter Notice</a>.**

**So that the complaining party can get back to you, please provide both your telephone number and physical address.**

[private]

[private]

**Please type your full legal name below to sign this request.**

[private]
