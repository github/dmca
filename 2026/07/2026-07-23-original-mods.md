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
   
I am the [private] [private] and [private] of the source code, custom game mechanics, and assets for the Minecraft modifications "Slyrien Omega", "Omega Miscellaneous", "Jujutsu Anomalies" (also published as "What If Anomalies") and "JJCLibraryMod". I hold [private] and [private] copyright ownership of these proprietary works.   
   
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**   
   
The copyrighted works are proprietary Minecraft modifications (mods) written in Java for Minecraft version 1.20.1, utilizing the Minecraft Forge and Mixin frameworks. I hold [private] copyright ownership of the following original works:   
   
1. "Slyrien Omega" & "Omega Miscellaneous" - These mods contain [private] original, custom-engineered detachable limb-loss system (often referred to internally as the "Gib" or "Limb" system), which includes custom capability data handling, coordinate math, joint-rotational physics, and specific 3D bone/flesh/muscle rendering shaders and layers.   
2. "Jujutsu Anomalies" (also published as "What If Anomalies") - Contains original designs, coordinate projection calculations, and client-side rendering handlers for custom in-game UI.   
3. "JJCLibraryMod" - An original, custom-coded API and library mod containing performance crash-guards, custom capability synchronization protocols, and utility managers designed to interface with modded combat pipelines.   
   
The disputed repository contains a systematic, wholesale copy of [private] proprietary modding codebase. Rather than independently writing code to replicate a feature, the infringer decompiled [private] closed-source binaries (which were distributed as compiled .jar files to beta testers on [private]) and copied entire packages, classes, and helper systems into their repository under their own namespace.   
The following comparative details provide direct, technical evidence of this infringement:   
1. Direct Copy of [private] Custom Limb Detachment & Regrowth System   
[private] proprietary "Gib/Limb" system developed for "Slyrien Omega" and "Omega Miscellaneous" was copied in its entirety. The infringer reproduced 25 individual classes belonging to this system under their own "net.mcreator.jujutsucraft.addon.limb" package, maintaining the exact same program architecture.   
Non-Functional Aesthetic Footprint (The "Smoking Gun"):   
In [private] original mod, severed limbs (" gibles") were designed to pulse with a white overlay tint via a sine-wave calculation (Math.sin(age * frequency)) to indicate an imminent explosion timer. In the infringing file "LimbLossHandler.java" and "SeveredLimbRenderer.java", they reproduced this exact visual, causing normal, non-explosive player and NPC severed limbs to continuously pulse with the same white tint. Replicating this non-functional, highly specific rendering quirk is concrete proof of unedited code reproduction via decompilation.   
Identical Mathematical Modeling:   
The infringer's "RegrowthBoneModel.java" and "RegrowthHeadAnatomy.java" contain identical local-space pivot models and coordinate vectors designed to render 3D player-model bones, skull, brain, and tissue layers. The segments, coordinate offsets, and mathematical scaling factors match mine exactly.   
2. Duplication of Core Utility & Math Logic   
Multiple unique math files and logic helpers have been copy-pasted with minor variable refactoring:   
Color Blending/Interpolation (ColorUtil.java vs. ReferenceStyle.java / ComboDisplay.java):   
The infringer's "blend" method is a direct duplicate of [private] original "interpolateColor" method. It extracts bitwise color channels, applies the same linear interpolation clamp, and utilizes the identical bitwise shift assembly.   
Reworked HUD Progress Bar (OverlayAPI.java vs. DustOverlayFormat.java):   
Both programs use the exact same loop structures and transition index logic to append formatting colors and color-reset control codes ("§r") at precise boundaries of a 10-segment HUD progress bar.   
Billboard Textbox Rendering (MangaBubbleClientHandler.java vs. TechniqueTextBoxClient.java):   
The infringer's text-box projection, matrix translations (-width / 2.0F, -height), text wrapping, word splitting based on RASTER_SCALE, and custom camera-shake calculations (shakeX, shakeY) are identical to the custom math [private] engineered.   
3. Duplication of API Libraries and Performance Guard rails   
The files "BaseCapabilityCrashGuardEvents.java", "ClientPacketHandler.java", and "CooldownTrackerEvents.java" duplicate the exact crash-prevention overrides, NBT variable handling, and capability synchronization rules [private] wrote to optimize performance under heavy modded environments.   
These extensive structural and functional duplicates demonstrate that the infringing repository relies entirely on unauthorized, plagiarized code extracted from [private] copyrighted work.   
   
**If the original work referenced above is available online, please provide a URL.**   
   
Type the official links to your projects:   
Jujutsu Anomalies CurseForge Link: https://www.curseforge.com/minecraft/mc-mods/what-if-anomalies   
JJCLibraryMod CurseForge Link: https://www.curseforge.com/minecraft/mc-mods/jjc-library   
Slyrien Omega CurseForge Link: https://www.curseforge.com/minecraft/mc-mods/slyrien-omega   
   
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**   
   
**Based on the above, I confirm that:**   
   
Specific files within the repository are infringing   
   
**Identify only the specific file URLs within the repository that is infringing:**   
   
Please remove the following specific infringing files from the repository "joinquit3110/JoQu-JJC-Addon":   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbLossHandler.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbGameplayHandler.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbCapabilityProvider.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbData.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbType.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbState.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbSounds.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbParticles.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbRegrowthLayer.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbRegrowthClientState.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbRenderHandler.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/ClientLimbCache.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/FirstPersonLimbRenderer.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/SeveredLimbEntity.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/SeveredLimbRenderer.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbEntityRegistry.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/LimbDebugCommands.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/RegrowthBoneModel.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/RegrowthBoxPatchModel.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/RegrowthHeadAnatomy.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/RegrowthHeadPatchModel.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/RCTLevel3Handler.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/RctAwakeningCutscene.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/NearDeathPacket.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/limb/JjcNpcLimbAllowlist.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/DustOverlayFormat.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/FugaCooldownClear.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/FugaDustLogic.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/FugaFireResult.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/FugaRewardState.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/HeaderLayout.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/Rect.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/SlashVfxPolicy.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/StaleStateReconciler.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/logic/SurehitState.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/clash/client/ClashHudOverlay.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/clash/client/ClientClashCache.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/TechniqueTextBoxClient.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/BaseCapabilityCrashGuardEvents.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/CeDebugCommands.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/ClientPacketHandler.java   
https://github.com/joinquit3110/JoQu-JJC-Addon/blob/main/src/main/java/net/mcreator/jujutsucraft/addon/CooldownTrackerEvents.java   
   
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**   
   
No   
   
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**   
   
**Is the work licensed under an open source license?**   
   
No   
   
**What would be the best solution for the alleged infringement?**   
   
Reported content must be removed   
   
**Do you have the alleged infringer’s contact information? If so, please provide it.**   
   
GitHub Username: joinquit3110   
[private] : [private]   
Email: Unknown   
   
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**   
   
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**   
   
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**   
   
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**   
   
**So that we can get back to you, please provide either your telephone number or physical address.**   
   
[private]
   
**Please type your full name for your signature.**   
   
[private]
