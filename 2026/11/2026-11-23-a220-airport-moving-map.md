Before disabling any content in relation to this takedown notice, GitHub  
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).  
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).  
  
To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).  
  
---  
  
Dear GitHub Copyright Team,  
  
I have read and understand GitHub's Guide to Filing a DMCA Notice.  
  
I am submitting this DMCA takedown notice as the [private] and copyright owner of the software “GM5 A220 Airport Moving Map”, published by [private] as freeware for Microsoft Flight Simulator.  
  
COMPLAINANT / COPYRIGHT OWNER  
  
Name:  
[private]  
  
Address:  
[private]  
[private]  
[private]  
  
Telephone:  
[private]  
  
Email:  
[private]  
  
1. COPYRIGHTED WORK  
  
[private] copyrighted work is:  
  
GM5 A220 Airport Moving Map  
  
Original publication:  
https://flightsim.to/addon/114744/a220-airport-moving-map  
  
Relevant original public release:  
GM5-A220-AMM-1.0.4-A220-1.0.8-RELEASE.zip  
  
Publication date:  
September 10, 2026  
  
Relevant original source file:  
  
gm5-a220-amm/html_ui/Pages/VCockpit/Instruments/a22x/DisplayUnits/gm5-a220-amm.js  
  
SHA-256 of the original release ZIP:  
  
77e769f757dffd2611ba9f1bd2a636347febfcac8ed09706bf97594d141cc464  
  
SHA-256 of the original gm5-a220-amm.js:  
  
0ca9b67f9f6a39e772ca0da6190683a3c56d123bbea7d644c179be95ebacf1ea  
  
[private] GM5 A220 Airport Moving Map is distributed as freeware. It is not open-source software.  
  
[private] have never granted the owner of the repository identified below permission to copy, modify, port, redistribute, sublicense, or relicense [private] GM5 source code.  
  
2. REPOSITORY CONTAINING THE MATERIAL  
  
Repository:  
  
https://github.com/Vihaan2012-cmyk/Free-Airport-Mapping-DB  
  
GitHub user:  
  
Vihaan2012-cmyk  
  
I am not alleging that the entire repository is a copy of [private] work.  
  
My complaint is specifically directed at identifiable files that contain literal portions of [private] source code and source-modification tooling expressly designed to copy and modify [private] complete GM5 add-on.  
  
3. INFRINGING FILE: tools/port_a220_amm.py  
  
Direct URL:  
  
https://github.com/Vihaan2012-cmyk/Free-Airport-Mapping-DB/blob/main/tools/port_a220_amm.py  
  
This file expressly identifies itself as a tool to port [private] product, the:  
  
“GM5 A220 Airport Moving Map”  
  
Its documented usage requires an:  
  
“<unzipped gm5-a220-amm folder>”  
  
The relevant portions include the following.  
  
A. Copying the complete GM5 package  
  
At approximately lines 27-31, the script creates a destination package named:  
  
zzz-gm5-a220-amm  
  
and executes:  
  
shutil.copytree(SRC, DST)  
  
This causes the user's complete GM5 add-on directory to be copied into a new package.  
  
B. Modifying the GM5 package identity  
  
At approximately lines 33-41, the copied GM5 manifest is opened and modified.  
  
The script changes the title of [private] add-on to:  
  
GM5 A220 Airport Moving Map (MSFS 2020 port, amdb-bridge)  
  
C. Opening and modifying [private] exact source file  
  
The script specifically locates and modifies:  
  
html_ui/Pages/VCockpit/Instruments/a22x/DisplayUnits/gm5-a220-amm.js  
  
This is the primary JavaScript source file of [private] GM5 A220 Airport Moving Map.  
  
D. Literal GM5 source code included in the repository  
  
At approximately lines 52-63, the script contains a literal copy of [private]:  
  
currentStoredAccessToken()  
  
function as a search-and-replacement target.  
  
The same function already appears in [private] original publicly released GM5 v1.0.4 source at approximately lines 324-329.  
  
The repository's copied source includes the same sequence and identifiers, including:  
  
[private]  
  
[private]
  
and:  
  
[private]
  
The repository then uses that copied source expression to locate and replace the corresponding code inside [private] program.  
  
E. Additional exact GM5 function matching  
  
The same porting script also contains the exact function signature:  
  
[private]
  
This function already exists in [private] publicly released GM5 v1.0.4 source at approximately line 2126.  
  
The script uses that exact source signature as another replacement target and automatically inserts bridge-specific changes into [private] source.  
  
This file therefore does substantially more than provide independent interoperability.  
  
It explicitly:  
  
- requires the user's GM5 add-on as input;  
- copies the complete GM5 package;  
- renames the copied package;  
- opens [private] exact JavaScript source file;  
- contains literal excerpts from [private] source as matching targets;  
- and automatically modifies those portions of [private] program to create a ported GM5 package.  
  
4. INFRINGING FILE: src/bridge/patcher.rs  
  
Direct URL:  
  
https://github.com/Vihaan2012-cmyk/Free-Airport-Mapping-DB/blob/main/src/bridge/patcher.rs  
  
This file contains additional code written specifically to identify and modify [private] GM5 A220 Airport Moving Map source.  
  
At approximately lines 151-155, the source explicitly identifies:  
  
GM5 A220 Airport Moving Map (Gunman5)  
  
The code then defines source patterns taken from [private] GM5 implementation.  
  
A. Literal source matching  
  
At approximately line 159, the file defines a source-matching constant containing [private] currentStoredAccessToken() implementation.  
  
At approximately line 160, it contains the exact signature of [private]:  
  
[private]
  
function.  
  
B. GM5-specific rewriting  
  
The subsequent code uses these patterns to rewrite portions of [private] GM5 JavaScript.  
  
At approximately line 164, it checks specifically for the GM5-specific identifier:  
  
GM5_A220_AMM  
  
C. Direct identification of [private] source filename  
  
At approximately lines 193-200, the patcher searches installed packages specifically for:  
  
gm5-a220-amm.js  
  
and verifies the presence of:  
  
GM5_A220_AMM  
  
before modifying the file.  
  
The source also references copies generated by:  
  
tools/port_a220_amm.py  
  
These names, identifiers and source patterns are specific to [private] add-on and demonstrate that this code was deliberately written against [private] GM5 source.  
  
5. CHRONOLOGY  
  
The relevant GM5 source existed publicly before the complained-of A220 porting implementation.  
  
[private] GM5 A220 Airport Moving Map v1.0.4 containing the identified functions was publicly released on September 10, 2026.  
  
The other project's Flightsim.to release appeared afterward, on September 16, 2026.  
  
The exact GM5 functions used as matching and replacement targets in the complained-of repository were therefore already present in my published source before the reported project appeared.  
  
6. NO LICENSE OR AUTHORIZATION  
  
[private] have not released the GM5 A220 Airport Moving Map source under the MIT License, GPL, or any other open-source license.  
  
[private] have not authorized Vihaan2012-cmyk or any contributor to this repository to:  
  
- reproduce portions of [private] GM5 source code;  
- distribute portions of [private] GM5 source;  
- copy [private] complete add-on package;  
- modify [private] gm5-a220-amm.js source;  
- create or distribute an automated port of [private] add-on;  
- sublicense [private] source;  
- or represent [private] copyrighted source as covered by the repository's MIT License.  
  
The fact that [private] distribute [private] add-on free of charge does not constitute permission to reproduce, modify, redistribute, port, sublicense, or relicense its source code.  
  
7. SCOPE OF MY CLAIM  
  
I want to make the scope of this notice precise.  
  
I am NOT claiming that every file in the repository infringes [private] copyright.  
  
I am also NOT claiming that the separately written file:  
  
packages/msfs-a220-amm/.../amdb-a220-amm.js  
  
is a complete copy of [private] gm5-a220-amm.js.  
  
My technical comparison does not support such a claim.  
  
This notice is deliberately limited to material that I can directly identify and substantiate:  
  
1. literal GM5 source excerpts contained in tools/port_a220_amm.py;  
  
2. literal GM5 source excerpts and GM5-specific source rewriting contained in src/bridge/patcher.rs;  
  
3. functionality whose express purpose is to copy [private] complete GM5 package and modify [private] copyrighted source to create a ported version.  
  
8. REMEDY REQUIRED  
  
To remedy the infringement, I request that the affected user remove the unauthorized GM5-derived material.  
  
At minimum, this requires:  
  
1. removal of tools/port_a220_amm.py, or removal from that file of all code that copies [private] GM5 package, contains literal portions of [private] source, or automatically modifies gm5-a220-amm.js;  
  
2. removal from src/bridge/patcher.rs of the literal copies of [private] GM5 source code and all GM5-specific functionality that identifies and rewrites gm5-a220-amm.js;  
  
3. removal of any duplicated copies of those same GM5 source excerpts from repository releases, compiled artifacts, or other files under the affected user's control.  
  
I am not requesting the removal of independently developed AMDB functionality merely because it performs a similar task.  
  
9. CONTACT INFORMATION FOR THE ALLEGED INFRINGER  
  
GitHub username:  
  
Vihaan2012-cmyk  
  
Repository:  
  
https://github.com/Vihaan2012-cmyk/Free-Airport-Mapping-DB  
  
I do not have additional verified contact information for the repository owner.  
  
10. GOOD-FAITH STATEMENT  
  
I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law. I have taken fair use into consideration.  
  
11. ACCURACY AND AUTHORITY STATEMENT  
  
I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.  
  
Electronic signature:  
  
[private]  
  
[private]  
[private]  
[private]  
  
Telephone:  
[private]  
  
Email:  
[private]  
  
Date:  
[private]  
  
[private]  
