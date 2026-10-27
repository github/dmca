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
  
I am the [private] and copyright holder of the open source project "xcode-copilot-server", published at https://github.com/theblixguy/xcode-copilot-server. The project is licensed under the MIT License with the copyright notice "Copyright (c) [private]".  
  
**Please provide a detailed description of the original copyrighted work that has allegedly been infringed.**  
  
"xcode-copilot-server" is an OpenAI-compatible proxy API server written in TypeScript that allows users to use GitHub Copilot in Xcode. It wraps the GitHub Copilot SDK and exposes it as an OpenAI-compatible API. The project includes HTTP route handlers for /v1/models and /v1/chat/completions endpoints (as well as Anthropic API endpoints), configuration management (with JSON5 config files supporting MCP servers, CLI tools, body limits, excluded file patterns, reasoning effort, and auto-approve permissions), SSE streaming, Xcode MCP bridge integration, user-agent filtering, and CLI argument parsing. The project has 96 commits and is currently on version 2.2.0, first published in 2026.  
  
**If the original work referenced above is available online, please provide a URL.**  
  
https://github.com/theblixguy/xcode-copilot-server  
  
**We ask that a DMCA takedown notice list every specific file in the repository that is infringing, unless the entire contents of the repository are infringing on your copyright. Please clearly state that the entire repository is infringing, OR provide the specific files within the repository you would like removed.**  
  
**Based on the above, I confirm that:**  
  
The entire repository is infringing  
  
**Identify the full repository URL that is infringing:**  
  
https://github.com/mobile-ar/xcode-assistant-copilot-sever  
  
**Do you claim to have any technological measures in place to control access to your copyrighted content? Please see our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice#complaints-about-anti-circumvention-technology">Complaints about Anti-Circumvention Technology</a> if you are unsure.**  
  
No  
  
**If you are reporting an allegedly infringing fork, please note that each fork is a distinct repository and <i>must be identified separately</i>. Please read more about <a href="https://docs.github.com/articles/dmca-takedown-policy#b-what-about-forks-or-whats-a-fork">forks.</a> As forks may often contain different material than in the parent repository, if you believe any of the repositories or files in the forks are infringing, please list each fork URL below:**  
  
**Is the work licensed under an open source license?**  
  
Yes  
  
**Which license?**  
  
mit-license  
  
**How do you believe the license is being violated?**  
  
The MIT License requires that "The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software."  
  
The infringing repository is a derivative work, a Swift port of [private] TypeScript project, that reproduces the architecture, logic, configuration schema, endpoint design, error messages, CLI options, security model, and documentation structure of [private] original work.  
  
However, the infringing repository's LICENSE file does not include [private] original copyright notice ("Copyright (c) [private]"). Instead, it contains only the infringer's own copyright information. By failing to preserve [private] copyright notice as required by the MIT License, the infringer has violated the license terms, and therefore has no valid license to distribute this derivative work.  
  
The derivative nature of the work is further evidenced by:  
  
1) The repository was created with the majority of code in a single commit, consistent with AI-assisted translation rather than independent development.  
(2) The project reproduces the same two API endpoints (/v1/models and /v1/chat/completions), the same configuration options (mcpServers, allowedCliTools, bodyLimitMiB, excludedFilePatterns, reasoningEffort, autoApprovePermissions) with the same defaults, the same CLI arguments (--port, --log-level, --config), the same security design (localhost binding, user-agent filtering), and copied error messages and logic throughout  
(3) The infringing repo only implements features up to [private] v1.0.4 release, while [private] project has since progressed to v2.2.0 with 96 commits — this pins the exact point in time the code was copied.  
(4) [private] project demonstrates original incremental development, while the infringing repo has only 8 commits.  
  
**What changes can be made to bring the project into compliance with the license? For example, adding attribution, adding a license, making the repository private.**  
  
The infringer must update their LICENSE file to include [private] original copyright notice: "Copyright (c) [private]". Their copyright line may remain alongside mine but cannot replace it. Additionally, I would request (though this is not strictly a license requirement) that they add attribution in their README acknowledging that this project is a Swift port of https://github.com/theblixguy/xcode-copilot-server.  
  
**Do you have the alleged infringer’s contact information? If so, please provide it.**  
  
Their GitHub profile is https://github.com/mobile-ar. I do not have other contact information.  
  
**I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law.**  
  
**I have taken <a href="https://www.lumendatabase.org/topics/22">fair use</a> into consideration.**  
  
**I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.**  
  
**I have read and understand GitHub's <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-takedown-notice/">Guide to Submitting a DMCA Takedown Notice</a>.**  
  
**So that we can get back to you, please provide either your telephone number or physical address.**  
  
[private]  
  
**Please type your full name for your signature.**  
  
[private]  
