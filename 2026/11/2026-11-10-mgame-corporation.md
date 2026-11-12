Before disabling any content in relation to this takedown notice, GitHub  
- contacted the owners of some or all of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).  
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).  
  
To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).  
  
---  
  
Dear GitHub Copyright Agent,  
  
We are legal counsel and authorized representatives of MGame Corporation, the developer and copyright owner of the online game Knight Online, in connection with the enforcement of its intellectual property rights relating to Knight Online.  
  
A copy of our Power of Attorney authorizing us to act on behalf of MGame Corporation in connection with intellectual property enforcement matters is attached to this notice.  
  
We submit this notification pursuant to the Digital Millennium Copyright Act ("DMCA") and request the prompt disabling/removal of the infringing content identified below.  
  
1. Copyrighted Work  
  
The copyrighted work at issue is the Knight Online computer game and, in particular, its original copyrighted software and source code, including the original client-side and server-side software, game assets, data files and other original copyrightable elements of the game.  
  
Knight Online is an MMORPG developed by MGame Corporation, which owns all copyrights in the game. The game is published in [private] by Game Cafe Services Bilişim ve İletişim Hizmetleri Ticaret Ltd.Şti  under the NTTGame name, and internationally by Game Services Inc., a United States company, through nttgameonline.com and Steam. The official Knight Online websites include:  
  
- https://www.nttgame.com/  
- https://www.nttgameonline.com/  
  
The copyrighted materials at issue include, without limitation, original computer program code comprising the Knight Online client and server software, the game's original assets (including 3D models, textures, terrain and map data, effects and interface graphics), game data tables, as well as other original game-related software and copyrightable materials.  
  
The copyrighted work is not licensed for reproduction, modification, publication, distribution, or incorporation into the projects identified below.  
  
2. Infringing GitHub Repositories  
  
We have identified the following publicly accessible GitHub repositories as containing and/or distributing unauthorized copies, derivative implementations, and/or portions of the copyrighted Knight Online software and materials:  
  
A. Open-KO / KnightOnline  
  
https://github.com/Open-KO/KnightOnline  
  
This repository publicly describes itself as follows: "OpenKO is an open source version of the old school Knight Online MMORPG. Specifically designed for version 1.298." It contains extensive client, server, engine, file I/O, game and related source-code components.  
  
The repository's own README (https://github.com/Open-KO/KnightOnline/blob/master/README.md) expressly states: "Starting with the original leaked source, we have updated to DirectX 9, added function flags so that various file formats may be supported while remaining backwards compatible, and much, much more." The same README states that "the goal of this project is to replicate official client functionality while preserving accuracy and compatibility with the official client and server", and that "We stick to the 1298/9 database schema." The repository therefore admittedly originates from, and is built on, the original Knight Online source code, which was never authorized for publication or distribution, and is designed to replicate the official Knight Online client and server.  
  
We therefore allege infringement with respect to the repository as a whole.  
  
B. ko4life-net / ko  
  
https://github.com/ko4life-net/ko  
  
This repository publicly identifies itself as "Open source development of the game Knight Online" and describes itself as "a reversed engineered old version of the game aiming to replicate the nostalgic experience we all once had". By its own description, the repository is a reproduction of the original Knight Online game client, server and associated tools, derived from the original software through reverse engineering. It further incorporates, as submodules, the ko4life-net/ko-assets and ko4life-net/ko-db repositories identified below, which distribute the original game assets and database materials.  
  
We therefore allege infringement with respect to the repository as a whole.  
  
C. ko4life-net / ko-assets  
  
https://github.com/ko4life-net/ko-assets  
  
This repository identifies its contents as "Knight Online assets used by the game" and describes them as "the official old Knight Online server and game assets". Its master branch is based on version 1097 of the game, and a separate branch (1298) contains what the repository describes as the official USKO 1298 version.  
  
The repository therefore publicly distributes copies of the original Knight Online game assets and data.  
  
We allege infringement with respect to the repository as a whole.  
  
D. ko4life-net / ko-db  
  
https://github.com/ko4life-net/ko-db  
  
This repository publicly identifies itself as containing "Knight Online database scripts, dumps, documentation and all the good stuff".  
  
The repository is incorporated into the ko4life-net/ko project as its database component and supplies the game data required to operate the reproduced Knight Online server, corresponding to the original game's database materials.  
  
We therefore allege infringement with respect to the repository as a whole.  
  
E. ZeusAFK / LibreKO  
https://github.com/ZeusAFK/LibreKO  
  
The repository publicly describes itself as a reimplementation of the Knight Online game client and server.  
  
The associated project website ([private]) further promotes the project as:  
"KNIGHT ONLINE, REBUILT IN THE OPEN"  
and describes it as:  
"A from-scratch reimplementation of the client and the server."  
  
The project provides a downloadable Windows client for its "public test server" and provides instructions for deploying its Login and Game server services.  
  
The project further states that its server implements a "retail-accurate TCP" protocol.  
  
By its own statements, the project is designed to replicate the original Knight Online client and server, to communicate using the original game's network protocol, and to operate with the original game's assets and data, which are decoded from the retail game files by the companion LibreKO-Assets tool identified below. The project is therefore an unauthorized adaptation of the Knight Online game and is designed to enable the reproduction and public use of its copyrighted content. The characterization of the project as a "from-scratch reimplementation" does not authorize the reproduction, adaptation or distribution of the copyrighted Knight Online software and materials.  
  
We therefore allege infringement with respect to the repository as a whole.  
  
F. ZeusAFK / LibreKO-Assets  
https://github.com/ZeusAFK/LibreKO-Assets  
  
This repository is publicly identified as an "Offline asset and data decoder" for converting game data into resources usable by the LibreKO client. Its README states that the tool "decodes an installed Knight Online client into engine-ready assets: meshes as glTF, textures as PNG, terrain as heightmaps and splat maps, effects and game tables as JSON", and that it includes a ".tbl cipher".  
  
The associated project website expressly states that its pipeline:  
"Decodes the retail formats and emits Godot-native resources."  
  
The website identifies, among others, the following formats:  
- .n3chr / .n3pmesh – meshes  
- .dxt – encrypted textures  
- .gtd / .gtt – terrain  
- .opd – object placement  
- .tbl – data tables  
- .fxb – effects  
  
The project therefore specifically implements functionality designed to decode, decrypt and convert the proprietary retail data formats of the original game, and thereby to reproduce the game's copyrighted models, textures, terrain, effects and data tables in new formats, without authorization.  
  
We therefore allege infringement with respect to the repository as a whole.  
  
3. Basis for the Infringement Claims  
  
The identified repositories are not merely projects inspired by the general concept or genre of an MMORPG.  
  
The repositories expressly identify themselves as Knight Online projects and, in several cases, expressly represent that they are based on the original Knight Online source code, or contain Knight Online server code, official game assets, databases, tools, or reverse-engineered implementations.  
  
These public representations by the repository owners themselves establish that the identified repositories reproduce, are based on, or are designed to reproduce and operate with the original Knight Online software and materials.  
  
The unauthorized materials include, as applicable, client-side and server-side source code, game-engine components, file and data processing components, game assets, database materials, and related software components derived from the original Knight Online software.  
  
Neither the repository owners nor the contributors identified above have been granted permission by the copyright owner or its authorized representative to reproduce, distribute, publish, modify, adapt, or publicly make available these copyrighted materials.  
  
4. Request for Removal  
  
We respectfully request that GitHub:  
1. Disable access to the repositories identified above;  
2. Remove or disable the infringing copyrighted materials contained therein;  
3. Disable the forks identified below; and  
4. Take any other action available under GitHub's DMCA policy with respect to these unauthorized distributions.  
  
We expressly allege that the entire contents of each repository identified above are subject to our infringement claim, rather than merely isolated files, because the repositories constitute projects built around, incorporating, or distributing unauthorized reproductions and/or derivative versions of the copyrighted Knight Online software and related protected materials.  
  
Forks: The repository network of Open-KO/KnightOnline exceeds 100 repositories. Based on the representative number of forks we have reviewed, we believe that all or most of the forks are infringing to the same extent as the parent repository, and we request that GitHub disable the entire network.  
  
With respect to the other repositories identified above, we have not included individual forks in this notice. To the extent that specific forks are investigated and determined to contain the same infringing materials, we reserve the right to submit supplemental notices identifying those forks individually.  
  
5. No Anti-Circumvention Claim  
  
This notification is submitted as a copyright infringement claim.  
  
We are not asserting a separate anti-circumvention claim under the DMCA in this notice, and we reserve the right to do so.  
  
6. Good Faith Statement  
  
I have a good faith belief that use of the copyrighted materials described above on the infringing web pages and repositories is not authorized by the copyright owner, or its agent, or the law. I have taken fair use into consideration.  
  
7. Accuracy and Authority Statement  
  
I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.  
  
We are authorized to act on behalf of MGame Corporation in connection with the enforcement of the relevant intellectual property rights, as evidenced by the attached Power of Attorney.  
  
8. Contact Information  
  
Copyright Owner / Principal:  
MGame Corporation  
  
Authorized Representative / Legal Counsel:  
[private]  
Özocak & Erdoğan Law Firm  
  
Power of Attorney:  
Attached to this notice.  
  
Signature:  
[private]  
[private]
