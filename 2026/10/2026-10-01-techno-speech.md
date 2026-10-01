While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).

---

One or more repositories in this DMCA takedown notice has been processed in accordance with GitHub's prohibition on sharing unauthorized product licensing keys, software for generating unauthorized product licensing keys, and/or software for bypassing checks for product licensing keys.

You can learn more in [GitHub's Acceptable Use Policies](https://docs.github.com/en/github/site-policy/github-acceptable-use-policies).

---

**Are you the copyright holder or authorized to act on the copyright owner's behalf? If you are submitting this notice on behalf of a company, please be sure to use an email address on the company's domain. If you use a personal email address for a notice submitted on behalf of a company, we may not be able to process it.**

Yes, I am the copyright holder.

**Are you submitting a revised DMCA notice after GitHub Trust & Safety requested you make changes to your original notice?**

Yes

**Please provide the Zendesk ticket number of your previously submitted notice. Zendesk ticket numbers are 7 digit ID numbers located in the subject line or body of your confirmation email.**

4672334

**Does your claim involve content on GitHub or npm.js?**

GitHub

**Please describe the nature of your copyright ownership or authorization to act on the owner's behalf.**

This is a revised notice, refiled as a single document at the request of [private] (ticket 4672334, message of 26 August 2026). It consolidates our original notice of 17 August 2026 with the further material we reported on 18 and 26 August. It supersedes those messages and is intended to be read on its own.

The copyrighted works are owned in their entirety by Techno-Speech, Inc. (株式会社テクノスピーチ), a company incorporated in [private] and headquartered in [private]. Techno-Speech, Inc. developed the works in-house and holds all rights in them. They have never been released under any open source or other public licence.

I am [private], [private] and  [private] ([private]) of Techno-Speech, Inc. I submit this notice on behalf of the company as its authorised  [private], from my [private] on our  [private] ( [private]).

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

This notice concerns TWO copyrighted works, both proprietary commercial software of Techno-Speech, Inc., and THREE infringing repositories. Each is set out below.

================================================================  
PART A — THE COPYRIGHTED WORKS  
================================================================. 

WORK 1: VoiSona  
A proprietary singing voice synthesis application for Microsoft Windows. It generates singing and speech from musical scores and lyrics using our proprietary synthesis engine together with our separately licensed voice libraries.

Version at issue: v1.18.0.5 for Windows (x64), distributed as "VoiSona.exe".  
File size: exactly 21,433,272 bytes  
SHA-256: a8109e783f40ef7196ac5ae1601f38807727969bcce0bc11b1c0df9edcb45c33

WORK 2: VoiSona Talk  
A separate commercial product — our proprietary text-to-speech application — sold and licensed independently of Work 1, distributed as "VoiSona Talk.exe".
File size: exactly 20,306,880 bytes  
SHA-256: 5ed1587451aa224b699e4bd1bafc55245350504f667cfd3bf2563e1d4e9eb238  
PE sections: 7 (.text .rdata .data .pdata _RDATA .rsrc .reloc)

Both are commercial software, distributed exclusively through our official channels. They are licensed to end users and never sold outright. Neither is open source and neither has ever been published under any public licence. Access to each application, and to each of our paid voice libraries, is controlled by the technological protection measures described later in this notice.

================================================================  
PART B — THE INFRINGING COPIES  
================================================================.    

--- B1. Modified copies of Work 1 (VoiSona) ---

(1) https://github.com/TomorrowX6/voisona/releases/download/V1.0/VoiSona.exe  
21,433,272 bytes  
SHA-256 9ccfeb5a19af98cb08ff942cbe19f042631633992b090aef95ce339bf9c99dd9

(2) https://github.com/TomorrowX6/voisona-offline/releases/download/offline/VoiSona.exe  
21,433,856 bytes  
SHA-256 ea8131afad6b3f4211eb4804960a961caf26162541a7638a04b509fcf968ac3a

A third copy was distributed under the tag "online" (21,433,272 bytes, SHA-256 91f9a08f732d3764c45d56a903513e6bf5cfa22210e9cfbdcdf54e572c7b4bea). The operator deleted that release after we filed; it had been downloaded 67 times when we last observed it.

PROOF FOR ITEM (1) AND THE DELETED "online" COPY — IN-PLACE PATCHING:  
Both were byte-for-byte the same length as our official binary (21,433,272 bytes), yet had different SHA-256 values, and differed from each other. An independently authored or recompiled program would not coincidentally match the exact byte length of our binary. This is the signature of in-place byte patching: overwriting bytes inside our own file without altering its length. The repository confirms it — the README of TomorrowX6/voisona sets out a table of EIGHTEEN patches, each specifying a virtual address within our binary, the original byte sequence there, and a replacement sequence OF IDENTICAL LENGTH. That is precisely why the length is preserved.

--- B2. Modified copy of Work 2 (VoiSona Talk) ---

(3) https://github.com/TomorrowX6/voisona-offline/releases/download/talk-offline/VoiSona.Talk.exe  
20,307,456 bytes  
SHA-256 3c5bd10cc690f7d5e5b8339c3836609f1dcc45e4abfaf2aa877974b648fc95c3

PROOF — THE REPOSITORY'S OWN PATCH SCRIPT REPRODUCES THIS FILE EXACTLY:  
The repository distributes the script that produces this file. It appends a PE section to our binary to hold a replacement server address. Applying that script's own arithmetic to our official binary gives:

20,306,880 our official binary  
+ 64 padding to the next file-alignment boundary (0x200)  
+ 512 the appended section (one 0x200 unit)  
= 20,307,456 exactly the size of the reported file

We obtained the reported file in an isolated environment and examined it statically, without executing it. Every element the script is written to produce is present:  
- Our binary has 7 PE sections; the reported file has 8.  
- The additional section is named ".offline", virtual size 30 bytes, raw size 512 bytes, characteristics 0xC0000040.  
- Its raw offset equals our official binary's size rounded up to the next 0x200 boundary, and it begins precisely at the end of our original binary's image — the added data starts where our own binary ends.  
- Those 30 bytes contain the string [private] , which redirects our licensing API to the user's own machine, where the repository's mock_server.exe answers instead.  
- The reported file still contains our own original strings, including the address of our genuine licensing API and the URLs of our product download page, manual, sign-up page, terms and privacy documents. The operator replaced the pointer to the API address rather than removing our data.

We are not aware of any explanation for this correspondence other than that our binary was used as the input.

--- B3. Circumvention tooling distributed alongside them ---

https://github.com/TomorrowX6/voisona-offline/releases/download/offline/mock_server.exe  
https://github.com/TomorrowX6/voisona-offline/releases/download/talk-offline/mock_server.exe  
(6,461,440 bytes, SHA-256 6992df21acc81e6a01ec2f6271e7e5b7307627b0256b555054ea473c5555031d — the two are byte-identical)

This is not a copy of our software. It is a purpose-built server that impersonates our licensing API on the end user's own machine. It is addressed in the anti-circumvention section below.

--- B4. The fork ---

https://github.com/xiawow/voisona — created 22 August 2026, owned by GitHub user xiawow.

Our original notice recorded that neither reported repository had any fork, and asked that action be taken before the material was forked. This fork was created after we filed. Per your policy, each fork is a distinct repository that must be identified separately, so we identify it here.

It contains a complete copy of the parent repository's contents: the README with the eighteen-patch table, three documents analysing our DRM, our voice library download endpoints and the falsification of our licence records, seven PowerShell scripts that apply, verify and deploy those patches to our executable, and a script that derives the key material protecting our encrypted voice library data. It also reproduces, in plaintext, our internal API authentication credentials, our cryptographic key material and our internal content identifiers.

It carries no release assets, so it does not contain the modified binaries. It does contain the circumvention tooling, the documentation for using it, and our leaked credential and key material.

================================================================  
PART C — SCALE OF DISTRIBUTION  
================================================================.

Download counts taken from the repositories' own release pages:

Asset 17 Aug 26 Aug  
voisona-offline offline/VoiSona.exe 39 102  
voisona-offline talk-offline/VoiSona.Talk.exe — 20  
voisona V1.0/VoiSona.exe 0 5  
voisona-offline offline/mock_server.exe 35 53  
voisona-offline talk-offline/mock_server.exe — 19
voisona V1.0/catalog.txt — 8

Adding the 67 downloads recorded by the deleted "online" release, unauthorised copies of our executables have been downloaded AT LEAST 194 times.

The repositories have been actively developed throughout. After we filed, the operator added support for Work 2, added two further of our commercial voice libraries to the forged licence catalogue (LeuR on 18 August, NurseRobot Type_T on 20 August, bringing the total to seven of our paid voice libraries presented as purchased), and added a GitHub Actions workflow that builds the circumvention tool.

================================================================  
PART D — GITHUB ACTIONS IS USED TO BUILD THE CIRCUMVENTION TOOL  
================================================================.

On 20 August 2026 the operator added .github/workflows/build.yml to TomorrowX6/voisona-offline. It has completed successfully twice.

The workflow runs on a GitHub-hosted windows-latest runner, builds mock_server.exe, and uploads it as a build artifact. It includes a verification step that fails the build unless one of our internal voice library identifiers is found embedded in the resulting binary.

We raise this because the repository is no longer only hosting the circumvention tool: GitHub's own compute is being used to produce and verify it. We appreciate that this may also engage your Actions terms of service, which is a matter for you rather than for our copyright claim.

================================================================  
PART E — NO SEPARABLE NON-INFRINGING CONTENT  
================================================================.

Beyond the binaries, the repositories consist of patch scripts, byte-level patch tables, a substitute licensing server, tooling that enumerates our content delivery domain to download our commercial voice libraries in bulk without any login, and documents explaining how to use all of the above. There is no separable non-infringing content in any of the three repositories, which is why we request removal of each in its entirety rather than of individual files.

**If the original work referenced above is available online, please provide a URL.**

https://voisona.com/  
https://voisona.com/talk/

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

The entire repository is infringing

**Identify the full repository URL that is infringing:**

https://github.com/TomorrowX6/voisona  
https://github.com/TomorrowX6/voisona-offline

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

Yes

**What technological measures do you have in place and how do they effectively control access to your copyrighted material?**

Both of our products incorporate the following technological protection measures. All are enabled by default and cannot be switched off by the user.

1. ACCOUNT AUTHENTICATION. On launch, the application authenticates the user's account credentials against our licensing server. Until authentication succeeds, the application does not make its editing functionality available. This controls access to the application itself.

2. PER-VOICE-LIBRARY LICENCE VALIDATION. Our voice libraries are separately purchased products. Following authentication, the application validates against licence records issued by our server whether the user holds a licence for each voice library, and whether trial restrictions apply to it. This controls access to each paid voice library.

3. TRIAL DURATION RESTRICTION. For any voice library the user has not purchased, the application restricts synthesis playback and audio export to a short fixed duration. This limits access to unpurchased content to a brief evaluation sample.

4. AUTHENTICITY VERIFICATION OF SERVER RESPONSES. The application verifies response headers returned by our licensing API, so that a substitute or impersonating server cannot satisfy the authentication and licensing checks.

5. DEVICE REGISTRATION. Authenticated installations are registered against the user's account and are subject to a per-account device limit.

The combined effect is that, in an unmodified installation, neither product can be used at all without a valid account, and our paid voice libraries cannot be used beyond a short evaluation sample without a purchased licence. These measures are the sole means by which we control access to our software and to our commercially licensed voice content.

**How is the accused project designed to circumvent your technological protection measures?**

All three repositories exist for the purpose of defeating the measures described above, and say so expressly.

--- 1. Binary patches to our executables ---

https://github.com/TomorrowX6/voisona distributes patches to our executable together with a README documenting each one by virtual address and byte sequence. Their stated and actual effects include:

- forcing our authentication routine to always report success, so the application becomes fully usable with no valid account at all (defeats measure 1);
- removing the sign-in requirement on a clean installation, so no credentials are ever requested (defeats measure 1);  
- forcing our trial-state determination to always report "not a trial", so every voice library present on the system is treated as purchased (defeats measure 2);  
- falsifying our licence records as parsed by the application, so unpurchased voice libraries are displayed in the user interface as purchased and are removed from the trial section (defeats measure 2);  
- extending the trial duration restriction for both playback and audio export (defeats measure 3);  
- disabling our application update check, which prevents the modified installation from being superseded by a corrected release.

The same repository also publishes, in plaintext, our internal API authentication credentials, the cryptographic key material and derivation procedure for our encrypted voice library data, and our internal content identifiers. We have deliberately not reproduced those values in this notice, because notices submitted to GitHub are published and doing so would compound the harm. They are in the README and the docs/ directory. We will provide them to GitHub through a confidential channel on request.

--- 2. Redirection of our licensing API and defeat of the authenticity check ---

https://github.com/TomorrowX6/voisona-offline distributes patch scripts that, for both of our products:

- append a PE section to our binary containing a replacement server address, and rewrite the instruction that loads our licensing API address so that it points at that replacement (defeats measures 1, 2, 3 and 5, because our server is never contacted);  
- overwrite the code that verifies the authenticity of our licensing API's response headers, replacing those checks with no-operation instructions (defeats measure 4, which exists precisely to stop a substitute server from being accepted).

--- 3. A ready-to-run substitute licensing server ---

The same repository distributes mock_server.exe as a pre-built binary. It impersonates our licensing API on the end user's own machine and returns a forged licence catalogue in which our paid voice libraries are marked as validly purchased outright, with the trial list emptied so that nothing appears as a trial. The catalogue presently contains seven of our commercial voice libraries. Used together with the patched executable, our software runs with full functionality while never contacting our servers at all.

--- 4. Tooling for bulk acquisition of our commercial voice content ---

The repository added two scripts that systematically enumerate candidate identifiers and version numbers for our commercial voice libraries against our content delivery domain, in order to discover and download them in bulk, including products the operator holds no licence for. The scripts' own comments state that this is done without any login. They were deleted two commits later but remain recoverable from the repository's git history.

--- 5. GitHub Actions used to build the circumvention tool ---

On 20 August 2026 a workflow was added at .github/workflows/build.yml. It runs on a GitHub-hosted windows-latest runner, builds mock_server.exe, and uploads it as an artifact. It has completed successfully twice. It includes a verification step that fails the build unless one of our internal voice library identifiers is found embedded in the resulting binary.

--- 6. Presentation to the public ---

TomorrowX6/voisona contains a disclaimer in Chinese describing the project as being for reverse engineering study. TomorrowX6/voisona-offline contains no disclaimer and is presented purely as a ready-to-use end-user package: pre-built binaries plus step-by-step installation instructions in English, with no analytical content. Its repository description advertises the outcome as "any-account login, 42 voices as purchased".

The fork https://github.com/xiawow/voisona reproduces the patch scripts, the eighteen-patch documentation, the key-derivation script and our leaked credential and key material. It carries no release assets, so it does not include the pre-built binaries, but it does distribute the circumvention tooling and the instructions for using it.

This material constitutes circumvention technology and devices offered to the public.

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

https://github.com/xiawow/voisona

**Is the work licensed under an open source license?**

No

**What would be the best solution for the alleged infringement?**

Reported content must be removed

**Do you have the alleged infringer’s contact information? If so, please provide it.**

TomorrowX6/voisona and TomorrowX6/voisona-offline are owned by GitHub user TomorrowX6 ( [private]), display name " [private]". That account's public profile lists no email address, company or location. The only contact information available to us comes from the  [private]  [private]  [private] in the reported repositories: the commit author email address [private]. Commits are also attributed to  [private]. All commit timestamps carry the [private] timezone offset.

The fork https://github.com/xiawow/voisona is owned by GitHub user xiawow ( [private]), display name " [private]". We have no contact information for that account beyond its  [private].

We have not contacted either user. Both are anonymous to us, the material is packaged for end-user consumption rather than for research, and we are concerned that advance notice would prompt further mirroring rather than removal. That concern has already been borne out: a fork was created five days after we filed.

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

 [private]

**Please type your full name for your signature.**

 [private]
