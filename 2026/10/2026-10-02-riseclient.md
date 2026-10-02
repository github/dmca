Note: Because the reported network that contained the allegedly infringing content was larger than one hundred (100) repositories, and the submitter alleged that all or most of the forks were infringing to the same extent as the parent repository, GitHub processed the takedown notice against the entire network of 190 repositories, inclusive of the parent repository.

---

To the GitHub Copyright Agent,  
  
I have read and understand GitHub's Guide to Filing a DMCA Notice.  
  
This is a new, complete, self-contained notice. It supersedes in its entirety my  
earlier ticket [private] as new forks have popped up  
   
  
1. IDENTIFICATION OF THE COPYRIGHTED WORK  
  
I am the copyright owner of the proprietary RiseClient 6.9.5 source code and  
associated software, a Minecraft client [private] authored and distribute only in  
compiled, obfuscated form.  
  
RiseClient 6.9.5 has never been published as source code. Its source has never  
been licensed, open-sourced, or otherwise made available for reproduction,  
decompilation, or redistribution. The work is identified by its distributed  
binary rise-client.jar and by its distinctive internal package namespace  
com.alan.clients, which appears throughout the infringing repositories.  
  
I have not authorized the operator of the repository NoHackClient/Rise-6.9.5, or  
the operator of any fork of that repository, to decompile, reproduce, publish,  
distribute, or make [private] proprietary source code publicly available.  
  
  
2. IDENTIFICATION OF THE INFRINGING MATERIAL  
  
Parent repository:  
  
https://github.com/NoHackClient/Rise-6.9.5  
  
The repository is an unauthorized reproduction and decompiled redistribution of  
RiseClient 6.9.5 in its entirety. It does not merely quote or reference [private] work.  
The repository is [private] work, decompiled and renamed.  
  
The repository states this openly in its own README and description:  
  
- Title: "Rise 6.9.5 but... OpenSource"  
- Under the heading "modify": "full offline, deobf, rename, fix, patch auth,  
  add toggle and more...."  
- Build output is named build/dist/rise-client.jar  
- Runtime system properties are named -Drise.auth.username,  
  -Drise.auth.autologin and -Drise.gameDir  
- The repository description reads: "fully deobf, renamed, optimized, and  
  ideaready....."  
  
"deobf" (deobfuscated) is an admission that the source tree was produced by  
decompiling [private] distributed obfuscated binary, and "patch auth" is an admission  
that [private] authentication and licensing controls were removed so that the work  
could be run without authorization.  
  
The infringement is not limited to isolated lines or files. The repository's  
default branch contains 1,741 tracked paths (files and directories), and the  
entire Java source tree under src/main/java/com/alan/clients/ is a decompiled  
reproduction of [private] work. Representative infringing files include:  
  
https://github.com/NoHackClient/Rise-6.9.5/blob/main/src/main/java/com/alan/clients/Client.java  
https://github.com/NoHackClient/Rise-6.9.5/tree/main/src/main/java/com/alan/clients/anticheat  
https://github.com/NoHackClient/Rise-6.9.5/tree/main/src/main/java/com/alan/clients/module  
https://github.com/NoHackClient/Rise-6.9.5/blob/main/build.gradle  
https://github.com/NoHackClient/Rise-6.9.5/tree/main/libs  
  
Because the whole repository is a reproduction of a single copyrighted work, no  
line-level identification is possible or meaningful. The correct unit of  
identification is the repository.  
  
Forks.  
  
I understand that GitHub does not automatically disable forks when disabling a  
parent repository, and that each allegedly infringing fork must be identified  
and investigated. I have enumerated the complete fork network via the GitHub API  
on [private]. It currently comprises the parent repository plus 172 forks,  
every one of which is listed individually in Section 8 below.  
  
I investigated the forks as follows. Using the GitHub API I compared each  
sampled fork's default-branch tree against the parent's, and confirmed the  
presence and byte size of the core infringing file  
src/main/java/com/alan/clients/Client.java. In the sample I reviewed,  
deliberately weighted toward forks that have been renamed to obscure their  
origin, every fork reproduced [private] copyrighted source tree:  
  
- TheBlueSkyARL/Onyx-Client - root tree byte-identical to parent  
- WsksFox/Rise-6.9.5 - root tree byte-identical to parent  
- chengyi-XXD/Rise-6.9.5 - root tree byte-identical to parent  
- ywsqlbvxx/DihClient - identical Client.java (13,818 bytes)  
- test7032/fork - identical Client.java (13,818 bytes)  
- Rgxaa/Rise-6.9.5-backup- - identical Client.java (13,818 bytes)  
- cxk666nb/Rise-6.9.5-LAOMAO - identical Client.java (13,818 bytes)  
- FallingSkyQwQ/idk - identical Client.java (13,818 bytes)  
- furryRL1337/-6.9.5- - identical Client.java (13,818 bytes)  
- Xouerr/Rise - identical Client.java (13,818 bytes)  
- FSOracle/Rice - identical Client.java (13,818 bytes)  
- Kahvq/Rise-6.7 - identical Client.java (13,818 bytes)  
- Hypixeophicfren/Hypixelophicfren2123fa - identical Client.java (13,818 bytes)  
- zeij-drive/Rise-6.9.5-fork - identical Client.java (13,818 bytes)  
- Minnie123special/Mifan - identical Client.java (13,818 bytes)  
- dinhtuanquang/Rise-6.9.5 - modified copy of [private] Client.java (13,882 bytes),  
  remainder of the tree unchanged  
  
Several sampled forks also carry the parent's own description verbatim: "fully  
deobf, renamed, optimized, and ideaready".  
  
Because the reported network is larger than one hundred (100) repositories and  
would be difficult to review in its entirety, I state, as provided for in  
GitHub's DMCA Takedown Policy:  
  
"Based on the representative number of forks I have reviewed, I believe that all  
or most of the forks are infringing to the same extent as the parent  
repository."  
  
I understand that this statement is made under the sworn statement in Section 7.  
  
The network membership list is here, and is enumerated in full in Section 8:  
  
https://github.com/NoHackClient/Rise-6.9.5/network/members  
  
  
3. REMEDY REQUESTED - STEPS THAT WOULD RESOLVE THE INFRINGEMENT  
  
To remedy the infringement, the repository owners would need to remove every  
unauthorized copy of [private] work: the entire decompiled src/ tree, the bundled libs/  
artifacts derived from [private] distribution, and the build configuration that  
reproduces rise-client.jar. Because the repositories consist of nothing but [private]  
copyrighted work and build scaffolding around it, removing the infringing  
material leaves no repository behind. There is no non-infringing subset that  
could be preserved by deleting individual files or lines.  
  
I therefore request that GitHub disable the entire network: the parent  
repository at https://github.com/NoHackClient/Rise-6.9.5 and all 172 forks  
enumerated in Section 8.  
  
I am also willing to accept, as an alternative, that the owners strip the  
repositories so that they contain no portion of [private] copyrighted source and no  
build that produces [private] client, though given the repositories' stated purpose I  
do not expect that to be practical.  
  
  
4. MY CONTACT INFORMATION  
  
Name: [private]  
GitHub username: [private]  
Email: [private]  
Telephone: [private]  
Physical address: [private]  
  
  
5. ALLEGED INFRINGERS  
  
Parent repository owner: GitHub account NoHackClient  
[private]
  
Parent repository:  
https://github.com/NoHackClient/Rise-6.9.5  
  
Fork owners: the 172 GitHub accounts identified by the URLs in Section 8.  
  
The parent repository's README directs users to a [private] invite  
([private]) as its only other contact channel. I do not have any  
further verified contact information for the repository owners.  
  
  
6. GOOD-FAITH STATEMENT  
  
I have a good faith belief that use of the copyrighted materials described above  
on the infringing web pages is not authorized by the copyright owner, or its  
agent, or the law. I have taken fair use into consideration.  
  
  
7. STATEMENT UNDER PENALTY OF PERJURY  
  
I swear, under penalty of perjury, that the information in this notification is  
accurate and that I am the copyright owner, or am authorized to act on behalf of  
the owner, of an exclusive right that is allegedly infringed.  
  
  
8. COMPLETE LIST OF INFRINGING REPOSITORIES  
  
Parent (1):  
  
https://github.com/NoHackClient/Rise-6.9.5  
  
Forks (172, enumerated via the GitHub API on [private]):  
  
  1. https://github.com/WsksFox/Rise-6.9.5  
  2. https://github.com/chengyi-XXD/Rise-6.9.5  
  3. https://github.com/dinhtuanquang/Rise-6.9.5  
  4. https://github.com/250xtc-coder/Rise-6.9.5  
  5. https://github.com/TheBlueSkyARL/Onyx-Client  
  6. https://github.com/xquj/Rise-6.9.5  
  7. https://github.com/xiaoyu777666/Rise-6.9.5  
  8. https://github.com/L1ZeZzz/Rise-6.9.5  
  9. https://github.com/hchAAA/Rise-6.9.5  
 10. https://github.com/qingjiuqwq/Rise-6.9.5  
 11. https://github.com/Win11notgood/Rise-6.9.5  
 12. https://github.com/Geoadt-ljy/Rise-6.9.5  
 13. https://github.com/ywsqlbvxx/DihClient  
 14. https://github.com/test7032/fork  
 15. https://github.com/lovelyxxi/Rise-6.9.5  
 16. https://github.com/Abzxc114514/Rise-6.9.5  
 17. https://github.com/hello-world-sudo-root/Rise-6.9.5  
 18. https://github.com/Rgxaa/Rise-6.9.5-backup-  
 19. https://github.com/Moon85CN/Rise-6.9.5  
 20. https://github.com/tahaoook/Rise-6.9.5  
 21. https://github.com/elite-taha/Rise-6.9.5  
 22. https://github.com/qvwd/Rise-6.9.5  
 23. https://github.com/cxk666nb/Rise-6.9.5-LAOMAO  
 24. https://github.com/Rxys-FATHER/Rise-6.9.5  
 25. https://github.com/FallingSkyQwQ/idk  
 26. https://github.com/rrrRex1336/Rise-6.9.5  
 27. https://github.com/xiaoyu777-coder/Rise-6.9.5  
 28. https://github.com/zxy168zxy168-max/Rise-6.9.5  
 29. https://github.com/OctopusCat101/Rise-6.9.5  
 30. https://github.com/porrige114/Rise-6.9.5  
 31. https://github.com/AQ84/Rise-6.9.5  
 32. https://github.com/CNLETI7N/Rise-6.9.5  
 33. https://github.com/ty55024/Rise-6.9.5  
 34. https://github.com/hywhywhyw114514/Rise-6.9.5  
 35. https://github.com/xiangsu1145/Rise-6.9.5  
 36. https://github.com/Paluster/Rise-6.9.5  
 37. https://github.com/furryRL1337/-6.9.5-  
 38. https://github.com/Unfairbyte/Rise-6.9.5  
 39. https://github.com/DiritingX/Rise-6.9.5  
 40. https://github.com/unYasl/Rise-6.9.5  
 41. https://github.com/koishi1337/Rise-6.9.5  
 42. https://github.com/Starry-cbz/Rise-6.9.5  
 43. https://github.com/WEIHENG2025/Rise-6.9.5  
 44. https://github.com/ziyan05618/Rise-6.9.5  
 45. https://github.com/dpdp1155/Rise-6.9.5  
 46. https://github.com/HeypixelMinecraft/Rise-6.9.5  
 47. https://github.com/Xouerr/Rise  
 48. https://github.com/error1688/Rise-6.9.5  
 49. https://github.com/dawfn/Rise-6.9.5  
 50. https://github.com/l-llllllllj/Rise-6.9.5  
 51. https://github.com/jett2q1/Rise-6.9.5  
 52. https://github.com/Ij1chi-Nijika/Rise-6.9.5  
 53. https://github.com/Fyew36/Rise-6.9.5  
 54. https://github.com/minexBruh/Rise-6.9.5  
 55. https://github.com/guiwowxx/Rise-6.9.5  
 56. https://github.com/Oulikuato1/Rise-6.9.5  
 57. https://github.com/rosecat2359/Rise-6.9.5  
 58. https://github.com/Zi0Ero/Rise-6.9.5  
 59. https://github.com/Esoxiii/rise-6.9.5  
 60. https://github.com/Dedfecss/Rise-6.9.5  
 61. https://github.com/0botyki/Rise-6.9.5  
 62. https://github.com/BlackDevCoding/rise-6.9.5  
 63. https://github.com/zyq2012zyq/Rise-6.9.5  
 64. https://github.com/fenbi2774/Rise-6.9.5  
 65. https://github.com/Haswell-refresh/Rise-6.9.5  
 66. https://github.com/ahuif/Rise-6.9.5  
 67. https://github.com/Smpaper998/Rise-6.9.5  
 68. https://github.com/AresuDR/Rise-6.9.5  
 69. https://github.com/FSOracle/Rice  
 70. https://github.com/polybeta2/Rise-6.9.5  
 71. https://github.com/LovelyDazai/Rise-6.9.5  
 72. https://github.com/Msy-ylzcy/Rise-6.9.5  
 73. https://github.com/Shiroha135/Rise-6.9.5  
 74. https://github.com/potato-36/Rise-6.9.5  
 75. https://github.com/wszgr114514/Rise-6.9.5  
 76. https://github.com/yywd123/Rise-6.9.5  
 77. https://github.com/cucumbertw/Rise-6.9.5  
 78. https://github.com/1433223tan/Rise-6.9.5  
 79. https://github.com/WangYang1337/Rise-6.9.5  
 80. https://github.com/astor1336/Rise-6.9.5  
 81. https://github.com/XingDiao1337/Rise-6.9.5  
 82. https://github.com/abcdxyz6969/Rise-6.9.5  
 83. https://github.com/Dl1447/Rise-6.9.5  
 84. https://github.com/nian-520/Rise-6.9.5  
 85. https://github.com/zorvq/Rise-6.9.5  
 86. https://github.com/Shoreiew/Rise-6.9.5  
 87. https://github.com/lhy1145/Rise-6.9.5  
 88. https://github.com/chinesedog99/Rise-6.9.5  
 89. https://github.com/haliChina/Rise-6.9.5  
 90. https://github.com/PouLiu/Rise-6.9.5  
 91. https://github.com/L1ngZh1/Rise-6.9.5  
 92. https://github.com/SUNNBT/Rise-6.9.5  
 93. https://github.com/rvwa/rise-6.9.5  
 94. https://github.com/Xylitol0429/Rise-6.9.5  
 95. https://github.com/uninstall400/rise-6.9.5  
 96. https://github.com/sxd91/Rise-6.9.5  
 97. https://github.com/J3y0r/Rise-6.9.5  
 98. https://github.com/storagepurpose9090/Rise-6.9.5  
 99. https://github.com/GenshinMark/Rise-6.9.5  
100. https://github.com/4hzzy/Rise-6.9.5  
101. https://github.com/454244513/Rise-6.9.5  
102. https://github.com/MicroWater/Rise-6.9.5  
103. https://github.com/Zis30axs/Rise-6.9.5  
104. https://github.com/InvShrk1/Rise-6.9.5  
105. https://github.com/yizhirexueya/Rise-6.9.5  
106. https://github.com/Baishanjun788/Rise-6.9.5  
107. https://github.com/Kahvq/Rise-6.7  
108. https://github.com/Iskuyal/Rise-6.9.5  
109. https://github.com/Mu1337/Rise-6.9.5  
110. https://github.com/Hypixeophicfren/Hypixelophicfren2123fa  
111. https://github.com/xxkej/Rise-6.9.5  
112. https://github.com/Jimsjdj/Rise-6.9.5  
113. https://github.com/lightfeather721/Rise-6.9.5  
114. https://github.com/z1337891506/Rise-6.9.5  
115. https://github.com/Today1337/Rise-6.9.5  
116. https://github.com/wlax-founder/Rise-6.9.5  
117. https://github.com/Quit1yru/Rise-6.9.5  
118. https://github.com/Go1denQwQ/Rise-6.9.5  
119. https://github.com/lmx0721/Rise-6.9.5  
120. https://github.com/WeiBei-SEN/Rise-6.9.5  
121. https://github.com/TR114514-QwQ/Rise-6.9.5  
122. https://github.com/banglee13/Rise-6.9.5  
123. https://github.com/zeij-drive/Rise-6.9.5-fork  
124. https://github.com/mxchu666/Rise-6.9.5  
125. https://github.com/GUKUAN/Rise-6.9.5  
126. https://github.com/xiaoaiXA/Rise-6.9.5  
127. https://github.com/dhsunisahentai/Rise-6.9.5  
128. https://github.com/lowmoonmk/Rise-6.9.5  
129. https://github.com/qinuan01/Rise-6.9.5  
130. https://github.com/idis0/Rise-6.9.5  
131. https://github.com/SUSRDev/Rise-6.9.5  
132. https://github.com/Swnsets/Rise-6.9.5  
133. https://github.com/KEpt1337/Rise-6.9.5  
134. https://github.com/Lostherat3596/Rise-6.9.5  
135. https://github.com/Cheaterawa/Rise-6.9.5  
136. https://github.com/Sup3rS061c/Rise-6.9.5  
137. https://github.com/ItLander/Rise-6.9.5  
138. https://github.com/xiaosu-b/Rise-6.9.5  
139. https://github.com/Fengmou1337/Rise-6.9.5  
140. https://github.com/Typro823827827/Rise-6.9.5  
141. https://github.com/wdaming163-sudo/Rise-6.9.5  
142. https://github.com/Ncyeowo-lab/Rise-6.9.5  
143. https://github.com/Hakuri1337/Rise-6.9.5  
144. https://github.com/trangphatmac-lgtm/Rise-6.9.5  
145. https://github.com/PuSajian/Rise-6.9.5  
146. https://github.com/Slingerspir/Rise-6.9.5  
147. https://github.com/yajimicx330/Rise-6.9.5  
148. https://github.com/wyx6669036/Rise-6.9.5  
149. https://github.com/Yitiaoxianyu-h/Rise-6.9.5  
150. https://github.com/UltraPanda-XX/Rise-6.9.5  
151. https://github.com/ColumbinaHyposeleniaYS/Rise-6.9.5  
152. https://github.com/jiuxian1337/Rise-6.9.5  
153. https://github.com/666NM/Rise-6.9.5  
154. https://github.com/YYYYHHHH421331/Rise-6.9.5  
155. https://github.com/The-hz/Rise-6.9.5  
156. https://github.com/spiderf5/Rise-6.9.5  
157. https://github.com/pabukK/Rise-6.9.5  
158. https://github.com/eyz2021/Rise-6.9.5  
159. https://github.com/dot-1337/Rise-6.9.5  
160. https://github.com/StellralMist/Rise-6.9.5  
161. https://github.com/ecxwxz/Rise-6.9.5  
162. https://github.com/XiaoCi233/Rise-6.9.5  
163. https://github.com/winfqqqq/Rise-6.9.5  
164. https://github.com/kiooop1/Rise-6.9.5  
165. https://github.com/NachoNeko8080/Rise-6.9.5  
166. https://github.com/Jared2013-arch/Rise-6.9.5  
167. https://github.com/Minnie123special/Mifan  
168. https://github.com/Sh4rdgel/rise-6.9.5  
169. https://github.com/fjftj/Rise-6.9.5  
170. https://github.com/XiaoHanHan233/Rise-6.9.5  
171. https://github.com/ImSideDev/Rise-6.9.5  
172. https://github.com/Cool114/Rise-6.9.5  
  
  
Electronic signature:  
  
[private]  
[private]  
