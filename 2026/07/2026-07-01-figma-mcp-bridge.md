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

I am the copyright holder and [private] of the MIT-licensed repository https://github.com/gethopp/figma-mcp-bridge and the associated source files identified in this notice.

**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**

The copyrighted work is the software project "figma-mcp-bridge", including its Go server implementation, plugin bridge architecture, websocket protocol implementation, leader/follower coordination system, plugin runtime integration, serializer implementation, and associated source code.

Repo URL:  
https://github.com/gethopp/figma-mcp-bridge/tree/82b28bc

The original work includes, among others, the following files:

- server-golang-backup/main.go  
- server-golang-backup/node/node.go  
- server-golang-backup/follower/follower.go  
- server-golang-backup/leader/leader.go  
- server-golang-backup/election/election.go  
- server-golang-backup/bridge/protocol.go  
- server-golang-backup/bridge/websocket.go  
- plugin/src/main/code.ts  
- plugin/src/main/serializer.ts

The downstream repository reproduces substantial portions of the structure, implementation, protocol design, message flow, bridge logic, serializer logic, and plugin communication architecture from these files.

**If the original work referenced above is available online, please provide a URL.**

https://github.com/gethopp/figma-mcp-bridge/tree/82b28bc

**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**

**Based on the above, I confirm that:**

Specific files within the repository are infringing

**Identify only the specific file URLs within the repository that is infringing:**

Original:     
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/server-golang-backup/main.go     
Allegedly infringing:     
https://github.com/vkhanhqui/figma-mcp-go/blob/main/cmd/figma-mcp-go/main.go   
   
Original:     
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/server-golang-backup/node/node.go   
Allegedly infringing:   
https://github.com/vkhanhqui/figma-mcp-go/blob/main/internal/node.go   
   
Original:   
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/server-golang-backup/follower/follower.go   
Allegedly infringing:   
https://github.com/vkhanhqui/figma-mcp-go/blob/main/internal/follower.go   
   
Original:   
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/server-golang-backup/leader/leader.go   
Allegedly infringing:   
https://github.com/vkhanhqui/figma-mcp-go/blob/main/internal/leader.go   
   
Original:   
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/server-golang-backup/election/election.go   
Allegedly infringing:   
https://github.com/vkhanhqui/figma-mcp-go/blob/main/internal/election.go   
   
Original:   
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/server-golang-backup/bridge/protocol.go   
Allegedly infringing:   
https://github.com/vkhanhqui/figma-mcp-go/blob/main/internal/types.go   
   
Original:   
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/server-golang-backup/bridge/websocket.go   
Allegedly infringing:   
https://github.com/vkhanhqui/figma-mcp-go/blob/main/internal/bridge.go   
   
Original:   
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/plugin/src/main/code.ts   
Allegedly infringing:   
https://github.com/vkhanhqui/figma-mcp-go/blob/main/plugin/src/main.ts   
   
Original:   
https://github.com/gethopp/figma-mcp-bridge/blob/82b28bc/plugin/src/main/serializer.ts   
Allegedly infringing:   
https://github.com/vkhanhqui/figma-mcp-go/blob/main/plugin/src/serializers.ts   

**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**

No

**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**

**Is the work licensed under an open source license?**

Yes

**Which license?**

mit-license

**How do you believe the license is being violated?**

The downstream repository distributes copied or substantially derived portions of the original MIT-licensed work without preserving the required upstream copyright and license notices.

The original project is licensed under MIT and requires that the copyright notice and permission notice be included in copies or substantial portions of the software.

The downstream repository currently attributes the upstream repository only as a "Related Project" while replacing the upstream copyright notice in the LICENSE file with a downstream-only copyright notice.

The identified files preserve substantial expressive structure from the original project, including the same bridge architecture, websocket protocol structure, leader/follower coordination model, request/response schema, plugin runtime flow, serializer logic, endpoint layout, and local server configuration.

**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**

The repository owner can remedy the violation by doing one of the following:

1. Restore license compliance by:  
- preserving the original upstream MIT copyright notice alongside any downstream notice;  
- clearly stating that the project is derived from / based on gethopp/figma-mcp-bridge;  
- adding prominent attribution in the README and/or a NOTICE / DERIVATIONS file; and  
- preserving attribution/license notices in copied or substantially derived files;

OR

2. Remove the copied or substantially derived files identified in this notice.

**Do you have the alleged infringer’s contact information? If so, please provide it.**

GitHub user: vkhanhqui  
Repository: https://github.com/vkhanhqui/figma-mcp-go

Prior communication regarding this issue:  
[private]

**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**

**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**

**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**

**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**

**So that we can get back to you, please provide either your telephone number or physical address.**

[private]

**Please type your full name for your signature.**

[private]
