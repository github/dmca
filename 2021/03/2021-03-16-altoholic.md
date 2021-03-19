**Are you the copyright holder or authorized to act on the copyright owner's behalf?**

Yes, I am the copyright holder.

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

Unauthorized repositories of World of Warcraft add-ons which I created in 2008 and published under an All Rights Reserved license have been uploaded on GitHub.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed. If possible, include a URL to where it is posted online.**

The add-on is named Altoholic, and there exists two versions: one for World of Warcraft "Retail", and one for World of Warcraft "Classic".
The add-on is also comprised of multiple libraries named "DataStore*" (see list below).

The add-on was first published on the WoW Interface website (2008) and later on the Curse website (2008). Curse is now owned by "Overwolf".

After I left the game in February 2020, I authorized modifications to the code under the condition that they respected Activision Blizzard's terms of service. The code was maintained from then until December 2020 by this user https://github.com/teelolws

In December 2020, this person breached the terms of services in two different ways :
https://us.forums.blizzard.com/en/wow/t/ui-add-on-development-policy/24534

1) He weaponized the main DataStore module by adding one line of code that would target another add-on's functionality (point 3: Add-ons must not negatively impact World of Warcraft realms or other players)

2) He obfuscated this line of code (point 2: Add-on code must be completely visible)
The obfuscation consisted in the addition of one line of Lua code using loadstring, visible here :
[private]  
The line which I do not have in its original form used a loadstring() with ascii codes instead of plain text, but translated to :
if AddOn_TotalRP3 then wipe(AddOn_TotalRP3) end
In World of Warcraft, this would test the presence of the add-on TotalRP3 (found here : Total RP 3 - Addons - World of Warcraft - CurseForge) and basically wipe its internal in-memory table in the game's Lua environment, thereby rendering the add-on unusable.
This environment is shared by the game's UI and all add-ons installed by the user, it is thus trivial to target other add-ons.  

**What files should be taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**

The following entire repositories should be taken down :

The 2 main add-on versions for WoW Retail & Classic  

https://github.com/teelolws/Altoholic-Retail  
[invalid]

.. and all the add-on libraries :  
https://github.com/teelolws/DataStore_Achievements  
https://github.com/teelolws/DataStore_Agenda  
https://github.com/teelolws/DataStore_Auctions    
https://github.com/teelolws/DataStore_Characters    
https://github.com/teelolws/DataStore_Containers  
https://github.com/teelolws/DataStore_Crafts  
https://github.com/teelolws/DataStore_Currencies    
https://github.com/teelolws/DataStore_Garrisons  
https://github.com/teelolws/DataStore_Inventory  
https://github.com/teelolws/DataStore_Mails  
https://github.com/teelolws/DataStore_Pets  
https://github.com/teelolws/DataStore_Quests   
https://github.com/teelolws/DataStore_Reputations  
https://github.com/teelolws/DataStore_Spells  
https://github.com/teelolws/DataStore_Stats  
https://github.com/teelolws/DataStore_Talents  

The main library, just called "DataStore" is no longer visible (most likely gone private since it contained the incriminating code) was probably here :
[private] (now a 404)

**Have you searched for any forks of the allegedly infringing files or repositories? Each fork is a distinct repository and must be identified separately if you believe it is infringing and wish to have it taken down.**

Yes, I have not found any.

**Is the work licensed under an open source license? If so, which open source license? Are the allegedly infringing files being used under the open source license, or are they in violation of the license?**

The work is open source, and is under an "All Rights Reserved" license.

I am the sole owner of all moral and economic rights on this work, and I use the paternity right to claim sole ownership on my work, and the right allow its distribution as I see fit.
The whole changelog.txt also proves that I have worked on this add-on since 2008.
My work can be found here : Altoholic - Addons - World of Warcraft - CurseForge

**What would be the best solution for the alleged infringement? Are there specific changes the other person can make other than removal? Can the repository be made private?**

Entire removal of all aforementioned repositories.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

No I do not.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

Phone number : [private]   

**Please type your full legal name below to sign this request.**

[private]  
