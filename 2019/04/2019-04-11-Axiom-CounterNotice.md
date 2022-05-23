Are you the owner of the content that has been disabled, or authorized to act on the owner’s behalf?   
Yes.

What files were taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL:   https://github.com/national/axiom-for-everyone

Do you want to make changes to your repository or do you want to dispute the notice?   
I'd like dispute the notice and restore the repository and its releases.

Is there anything else you think we should know about why you believe the material was removed as a result of a mistake? (optional)   
This software, Axiom, is compiled from a codebase that is based off of the Asuna codebase, which is open-sourced under the GPL license, therefore Axiom is a derivative work and is free software also covered by GPL. Therefore this DMCA takedown is invalid. Asuna is here [private] Many commits to Asuna have been reverted before taking it closed source, so that Axiom could not itself be DMCA'd by other contributors to Asuna. This is a fork of Asuna with those additional contributors still included https://github.com/national/asuna-for-everyone and some of that code is in Axiom but not all of it. However, since the original codebase is GPL, this means that DMCA cannot be used to take down a derivative work off of Axiom (the release on the axiom-for-everyone repo, whose takedown we are disputing here).

There are plenty of screenshots attached providing proof that Axiom is a modification of Asuna, all of the GitHub screenshots are from asuna-for-everyone, and the others can be verified with a Java bytecode viewer as being from the Axiom jar that was taken down from our repo. Specifically, from this folder https://github.com/national/asuna-for-everyone/tree/d19708e54720aa7dfdbbcf65176623b255f0afb3/src/main/java/com/sasha/asuna/mod/mixin/client as it is not obfuscated. As you can see, this code is GPL licensed. Almost all other files are demonstrably from Asuna, but the mixin folder is the easiest to demonstrate since it cannot be obfuscated and remain functional.

If you look on the top level of the Axiom jar, you can see that a file is still called "AsunaThumb.png". It it can be found at https://github.com/national/asuna-for-everyone/blob/d19708e54720aa7dfdbbcf65176623b255f0afb3/src/main/resources/AsunaThumb.png, and the shasum of the file in the Axiom jar matches (6ffbdcb930d2d6bad0565f703d2098d8509ef849).

Furthermore the configuration file which Axiom loads is still called AsunaData.yml. For example, see the string "You haven't defined any greeter messages in the AsunaData.yml file, located in your .mc folder. You should go there and add some!" in the Axiom jar is clearly the same as the error message in Asuna defined here https://github.com/national/asuna-for-everyone/blob/d19708e54720aa7dfdbbcf65176623b255f0afb3/src/main/java/com/sasha/asuna/mod/feature/impl/JoinLeaveMessagesFeature.java#L114

Here are the screenshots of identical Mixin annotation headers:   
[private]

There's also a possibility that the notice was filed fraudulently by a non adult under a false name and address.

Type (or copy and paste) the following statement: "I swear, under penalty of perjury, that I have a good-faith belief that the material was removed or disabled as a result of a mistake or misidentification of the material to be removed or disabled."   
I swear, under penalty of perjury, that I have a good-faith belief that the material was removed or disabled as a result of a mistake or misidentification of the material to be removed or disabled.

Type (or copy and paste) the following statement: "I consent to the jurisdiction of Federal District Court for the judicial district in which my address is located (if in the United States, otherwise the Northern District of California where GitHub is located), and I will accept service of process from the person who provided the DMCA notification or an agent of such person."   
I consent to the jurisdiction of Federal District Court for the judicial district in which my address is located (if in the United States, otherwise the Northern District of California where GitHub is located), and I will accept service of process from the person who provided the DMCA notification or an agent of such person.

Please confirm that you have you have read our Guide to Submitting a DMCA Counter Notice: https://help.github.com/articles/guide-to-submitting-a-dmca-counter-notice/

So that the complaining party can get back to you, please provide both your telephone number and physical address:   
[private]

Please type your full legal name below to sign this request:   
[private]
