**Are you the copyright owner or authorized to act on the copyright owner’s behalf?**  
Yes, I am the copyright owner.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed. If possible, include a URL to where it is posted online.**   
The original work is available for download as a compiled .NET executable:  
Version 3: https://donutteam.com/downloads/P3DEditor3/  
Version 4: https://donutteam.com/downloads/Pure3DEditor4/

Using ILSpy or .NET Reflector, both .NET decompilation tools, anyone is able to view decompiled C# code.

Original code, made in vb.net:  
[private]

Using ILSpy, this is the decompiled code:  
[private]

This same function is available in HandsomeMatt's Pure3D Repository:  
[private]

* vb.net and C# are both compiled into IL, meaning that this decompilation process is able to interpret it in either language.  
* Local variables inside a method are not available when decompiling ILSpy/.NET Reflector, so this would require [private] to name them himself. 
* ILSpy/.NET Reflector put placeholder names for variables. These placeholders are "num", "num2", "num3" and so on. Looking at [private]'s repository, you can notice he uses ``num1`` as a local variable inside his Read function. In the original code, this variable is named ``ChunkStart``.  
* All exceptions in [private]'s code are the same as the original code.     
* [private] named a variable ``chunkSize`` where the original code refers to it as ``EntireSize``; however reading the first exception throws the text "Header size greater than *entire size*."  

**What files should be taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL:**  
https://github.com/handsomematt/Pure3D/blob/master/src/Pure3D/Chunk.cs

**Have you searched for any forks of the allegedly infringing files or repositories? Each fork is a distinct repository and must be identified separately if you believe it is infringing and wish to have it taken down.**  
Yes, there are none visible on GitHub.

**Is the work licensed under an open source license? If so, which open source license? Are the allegedly infringing files being used under the open source license, or are they in violation of the license?**  
No, it is not licensed under an open source license.

**What would be the best solution for the alleged infringement? Are there specific changes the other person can make other than removal?**  
We request that they add a credit to the readme.md file and infringing files. The credit should include a link back to our [website](https://donutteam.com).

Otherwise, remove the infringing code.

**Do you have the alleged infringer’s contact information? If so, please provide it:**  
Email: [private]

I've attempted to resolve this issue with [private], however that was unsuccessful.

**Type (or copy and paste) the following statement: "I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law. I have taken fair use into consideration."**  
I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law. I have taken fair use into consideration.

**Type (or copy and paste) the following statement: "I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed."**  
"I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.

**Please confirm that you have you have read our Guide to Submitting a DMCA Takedown Notice: https://help.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/**  
I confirm that I have read GitHub's Guide to Submitting a DMCA Takedown Notice.

**So that we can get back to you, please provide either your telephone number or physical address:**  
Phone number is: [private]   
Email is: [private]

**Please type your full legal name below to sign this request:**   
[private]
