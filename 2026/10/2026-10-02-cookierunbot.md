While GitHub did not find sufficient information to determine a valid anti-circumvention claim, we determined that this takedown notice contains other valid copyright claim(s).  
  
---  
  
One or more repositories in this DMCA takedown notice has been processed in accordance with GitHub's prohibition on sharing unauthorized product licensing keys, software for generating unauthorized product licensing keys, and/or software for bypassing checks for product licensing keys.  
  
You can learn more in [GitHub's Acceptable Use Policies](https://docs.github.com/en/github/site-policy/github-acceptable-use-policies).  
  
---  
  
To whom it may concern,  
  
I have read and understand GitHub's Guide to Filing a DMCA Notice.  
  
I am the [private] and copyright owner of the software CookieRunBot, a commercial Windows application that [private] develop and sell to licensed customers. I am submitting this notice under 17 U.S.C. § 512(c). I have a good faith belief that the material identified below is infringing [private] copyright and is not authorized by me, my agent, or the law.  
  
This notice also includes a circumvention claim under 17 U.S.C. § 1201; the detail GitHub requires for such claims is set out in section 3.  
  
1. The copyrighted work  
CookieRunBot — a proprietary commercial Windows application (Python source compiled and distributed as a single obfuscated executable). It is not open source, carries no public licence, and is sold only to paying customers who receive a signed, machine-bound activation key.  
  
Official product and point of sale: https://cookieafk.com  
Official release distribution: https://github.com/divisition/ckr-bot/releases  
The specific version copied — v2.2.4 — was published by [private] on 2026-07-23.  
[private] private development repository containing the original source has commit history authored by [private] beginning 2026-07-03.  
I am the [private] of all of it. [private] have never licensed, open-sourced, or otherwise authorized redistribution of this software or its source code.  
  
2. The infringing material  
Repository: https://github.com/Beonneon/CRC  
  
Release asset (a modified copy of [private] executable): https://github.com/Beonneon/CRC/releases/tag/v2.2.4-community  
  
The repository was created on 2026-07-29, six days after [private] published v2.2.4. It infringes in three distinct ways:  
  
(a) Redistribution of [private] executable. The Releases page distributes a file named CookieRunBot.exe (~105 MB) that is a modified copy of [private] commercial product. The repository's own BUILDING.md identifies [private] original binary by exact filename, byte size, and SHA-256 hash (39fb30ecf5d44a8e133a84fdd03eb0927bffde4174f3ca55a3a9f98cd6ca6705) and instructs users to supply it as build input — an admission that the work is derived directly from [private] copyrighted binary.  
  
(b) Publication of [private] source code. The source/ directory contains [private] proprietary Python source recovered by decompiling [private] executable. The repository's README.md states this openly: "The ordinary modules are provided as readable recovered source." The files panel_web.py, updater.py, notify.py, supervisor.py, hotkeys.py, capture.py, record_profiles.py, and calibrate_templates.py are [private] original copyrighted works, reproduced without permission. The assets/ directory likewise reproduces [private] original web interface files and [private] image-template assets verbatim.  
  
(c) Distribution of [private] compiled modules. The compat/native/ directory redistributes compiled native modules extracted from [private] executable.  
  
3. Circumvention of a technological protection measure  
Beyond ordinary infringement, this repository exists specifically to defeat the licensing system that protects [private] work, and it distributes the tools to do so. This implicates 17 U.S.C. § 1201. GitHub asks for three specific points on a circumvention claim; they are addressed in order below.  
  
(i) What the technical protection measures are. Access to CookieRunBot is controlled by a licensing system with four components: (a) activation keys signed with an Ed25519 private key that [private] alone hold, which the application verifies against an embedded public key; (b) binding of each key to a single machine via a hardware fingerprint; (c) local licence state sealed with an HMAC so it cannot be edited or copied to another machine; and (d) a periodic online authorization check against [private] server, which can withdraw authorization for a machine.  
  
(ii) How they control access to the copyrighted work. The application refuses to operate without a valid, unexpired, machine-bound licence. A user who has not purchased a key cannot run it: the licence check gates the user interface at launch and the automation engine before each run. These measures are the sole means by which access to [private] software is restricted to paying customers.  
  
(iii) How the accused project circumvents them. The repository disables all four measures and publishes the tooling to do so. Its README.md states plainly: "Activation, heartbeat checks, and machine binding are disabled." The specific mechanism is documented in their own files:  
  
source/local_licensing_stub.py — replaces [private] licensing module with a stub whose check() unconditionally returns success, described in its own docstring as performing "no activation, heartbeat, machine binding, registry access, or license-file writes."  
source/local_only_runtime_guard.py — injected code that disables [private] authorization check and blocks [private] software from contacting [private] license server.  
scripts/repack_local_only.py and scripts/build.ps1 — tooling published for the express purpose of stripping the licensing module out of [private] executable and repacking it. Their own README.md describes the process as one that "removes the native licensing module."  
These files are offered to the public as a means of circumventing access and copy controls on [private] commercial software.  
  
4. Requested action and remediation  
Please remove or disable access to the entire repository https://github.com/Beonneon/CRC, including its Releases and all associated release assets.  
  
Remediation steps available to the user: none short of full removal would resolve this. Every component of the repository is either [private] copyrighted work (the recovered source in source/, the web interface and image templates in assets/, the redistributed executable in Releases) or tooling whose only function is to strip the licensing from [private] executable (scripts/). There is no subset of files the user could delete to bring the repository into compliance, and no attribution or licence notice would cure it, because the work is proprietary and [private] have never licensed it for redistribution. I am therefore requesting removal of the repository in its entirety rather than of individual files.  
  
I additionally request that any forks of this repository be disabled, per GitHub's forking policy.  
  
5. The alleged infringer  
GitHub username: Beonneon — https://github.com/Beonneon  
I have no relationship with this user and have never granted them any licence or permission with respect to this software.  
6. My contact information  
Full legal name: [private]  
Mailing address: [private]  
Telephone: [private]  
E-mail: [private]  
GitHub username: divisition  
7. Required statements  
I have read and understand GitHub's Guide to Filing a DMCA Notice.  
  
I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law. I have taken fair use into consideration.  
  
I swear, under penalty of perjury, that the information in this notification is accurate and that I am the copyright owner, or am authorized to act on behalf of the owner, of an exclusive right that is allegedly infringed.  
  
Signed,  
[private] [private]  
