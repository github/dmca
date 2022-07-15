**Are you the owner of the content that has been disabled, or authorized to act on the owner’s behalf?**

Yes, I am the content owner.

**Please describe the nature of your content ownership or authorization to act on the owner's behalf.**

Qurre is a plugin loading framework for the game on Unity - SCP: Secret Laboratory. It is distributed semi-open source (public methods and fields used by plugins are distributed in the repository)  
Qurre was created as a framework for developers and is supplemented as needed by developers in certain functions (controllers, methods, etc.)

**What files were taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**

https://github.com/Qurre-Team/Qurre-sl

**Do you want to make changes to your repository or do you want to dispute the notice?**

Dispute the notice.

**Is there anything else you think we should know about why you believe the material was removed as a result of a mistake?**

A very long time ago, in the summer of 2020, after another update of Exiled (another framework) that broke plugins, I decided to fork Exiled v1 (like LTS) and continue support it, but the repository was blocked, which was basically fair. After that, I decided to create my own framework, which would be most convenient for developers and where there would be no need to update plugins every half a year, which is costly for server hosts. Somewhere in the middle of 2021, when some servers began to switch to Qurre, users of exiled (including contributors) started raiding the Qurre discord server, accusing Qurre of stealing for no reason, and all that.  
In May of this year, I released the Qurre v1.14 update, in which I fixed the audio controller, I started this update in early May. Prior to this update, the audio worked while waiting for the players, but after the start of the round, it was not displayed to the players. (Which contradicts [private] words in his DMCA request) Then I asked another Qurre developer about his possible thoughts In a short conversation, relying on the fact that the audio was working in waiting of the players, he suggested a possible cause of the bug, and then I fixed it. (Screenshot on Russian language: [private] )  

Another point of accusation is the ability to play mp3. I was suggested to make mp3 support by user Qurre, who is also a plugins developer (Screenshot in Russian: [private] ) Adding mp3 support was not problematic, because I already knew about the NLayer library since 2020 (Screenshot: [private] ) This library converts mp3 to wav audio format. When I tried to play audio, it didn't play correctly. In order to understand whether the problem is NLayer or Audio Controller, I recorded the outgoing Stream from NLayer. When creating an audio controller through the Dissonance microphone, the developer must be aware that it works in one channel, from which it is not difficult to draw a logical conclusion that it is necessary to check the number of channels of output audio. After checking this on the first site we came across, we find out that the output audio has multichannel even with a certain NLayer setting. After that, by mental processes, we understand that we need to check the Audio Decoder in NLayer, we are looking for a class and a method, we see the if check for a one channel, which does not work, and we fix it. That's it, NLayer is fixed. Also, in the NLayer code itself, comments in this method says that it is can occur errors for multichannel, so it was easy to find it.

Another point he complained about was the release date of the update. As I said earlier, I started doing the update before the release date of his plugin. Also, as I said earlier, Qurre is a framework that develops as developers need it. In May I was updating and fixing my plugins, and I had a plugin that should play audio during the round. That is why I started the development of this update at that time.

Another of his complaints, in which he wrote that he "spent several hours to understand" - I don’t understand what his problem is to analyze the NLayer code and read the documentation and do a simple check on the remaining stream size, 10 minutes of searching is the maximum.

Complaining about using game methods/Dissonance methods is extremely stupid since we are creating code for this game that uses Dissonance, so it makes sense that we would use the same methods. This is the same as blaming some code for using system methods, for example: "Where", "foreach", etc.

His other complaints like: "his can loop his audio, he stole it" and incomprehensible complaints about "" in the Qurre code - it's useless to comment, because if you are at least a little able to analyze and understand the code, it will become clear that his request was submitted without a thorough the reasons.

After I made the v1.14.1 update, which added mp3 support, I submitted it to other Qurre developers for review. After that, one of the former developers told me: "[private] will 100% start accusing you of stealing, add a frame size and data update rate like his, so that he will definitely angry", which I basically did, because this is not DMCA. Since blocking the repository for this is equivalent to blocking a random repository where there is an int 10 in the code.

After the release of this update, [private] entered the discord server and started insulting all Qurre devs and accusing them of stealing. After that, exiled devs began to offer pull requests with the removal of all code from the repository, while insulting other qurre devs.

Due to the fact that this blocking has no reason, I would prefer if the repository was unbanned immediately. For all the accusations of [private], it is clear that he sent a complaint because of a personal offense.

**I swear, under penalty of perjury, that I have a good-faith belief that the material was removed or disabled as a result of a mistake or misidentification of the material to be removed or disabled.**

**I consent to the jurisdiction of Federal District Court for the judicial district in which my address is located (if in the United States, otherwise the Northern District of California where GitHub is located), and I will accept service of process from the person who provided the DMCA notification or an agent of such person.**

**Please confirm that you have you have read our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-counter-notice">Guide to Submitting a DMCA Counter Notice</a>.**

**So that the complaining party can get back to you, please provide both your telephone number and physical address.**

[private]

**Please type your full legal name below to sign this request.**

[private]
