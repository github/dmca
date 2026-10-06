**Are you the owner of the content that has been disabled, or authorized to act on the owner’s behalf?**

Yes, I am the content owner.

**Please describe the nature of your content ownership or authorization to act on the owner's behalf.**

**What files were taken down? Please provide URLs for each file, or if the entire repository, the repository’s URL.**

https://github.com/IQLvbs/openpilot

**Do you want to make changes to your repository or do you want to dispute the notice?**

Dispute the notice.

**Please explain why you believe the material was removed as a result of a mistake or misidentification.**

I believe the material was removed as the result of a mistake or misidentification because the takedown notice designates the entire IQ.Pilot repository as infringing sunnypilot material, although the repository contains extensive comma.ai openpilot code, separately owned third-party code, and IQ.Pilot-authored material that SUNNYPILOT LLC does not own. The notice also relies on concrete file mappings and technical assertions that are demonstrably incorrect for the relevant version of IQ.Pilot.

RELEVANT VERSIONS

This response concerns IQ.Pilot commit a7892c6. The comparison versions are sunnypilot commit 2c334ede443d7391d27575af8c854a095ba702a8 and comma.ai/openpilot commit 2895346746634d7eec0ee749f946c87039948a25. All file, path, and identifier statements below refer to those specific versions.

1. THE NOTICE'S ENTIRE-REPOSITORY DESIGNATION IS MATERIALLY OVERBROAD.

IQ.Pilot is based on comma.ai's openpilot:

https://github.com/commaai/openpilot

IQ.Pilot contains extensive stock openpilot material, separately licensed third-party components, and IQ.Pilot-specific implementation. SUNNYPILOT LLC does not acquire copyright ownership of comma.ai code merely because sunnypilot is also an openpilot fork. Nor does it own third-party code or expression independently authored for IQ.Pilot.

This distinction is required by 17 U.S.C. sections 102(b) and 103(b). Copyright does not extend to ideas, procedures, processes, systems, or methods of operation. Copyright in a derivative work extends only to new material contributed by that author and does not enlarge the author's rights in preexisting material. A valid comparison therefore must separate any protectable sunnypilot-authored expression from stock openpilot, third-party work, functional and interface-required elements, and IQ.Pilot's own implementation.

GitHub's official guidance makes the same software-specific distinctions. It states that a repository may contain code from many people while only one file or even one subroutine presents an infringement issue; that code combines functionality and expression, but copyright protects expression rather than functionality; and that applicable open-source licenses must be considered:

https://docs.github.com/en/site-policy/content-removal-policies/guide-to-submitting-a-dmca-takedown-notice

GitHub's DMCA policy further explains that an allegation that an entire repository infringes causes GitHub to skip the ordinary opportunity to modify identified files and disable the entire repository expeditiously:

https://docs.github.com/en/site-policy/content-removal-policies/dmca-takedown-policy

The claimant expressly selected "The entire repository is infringing." That designation caused extensive material not owned by the claimant to be disabled together with the specific passages actually disputed.

2. THE NOTICE'S LEADING HEADER-REPLACEMENT EXAMPLES DO NOT DESCRIBE IQ.PILOT COMMIT A7892C6.

The notice states that sunnypilot/__init__.py became a 95%-identical iqpilot/__init__.py. Sunnypilot's file contained functional utilities, hashing, and enum helpers. IQ.Pilot's package-initializer file was only a short copyright docstring and did not contain the alleged shared functional implementation. The files were not 95% identical as claimed.

IQ.Pilot commit a7892c6 did not contain the alleged iqpilot/selfdrive/ui/quiet_mode.py file, although the notice identifies it as a 100%-identical copy.

IQ.Pilot commit a7892c6 did not contain the alleged iqpilot/livedelay/helpers.py or iqpilot/livedelay/lagd_toggle.py files. It used openpilot's current live-delay system instead.

The notice describes the entire IQ.Pilot mapd directory as nine files with 85-98% similarity. That directory-wide characterization is false. IQ.Pilot contains materially different map and tile management, a substantially different manager/orchestrator, and short integration wrappers constrained by the third-party interface. Similarity in limited integration utilities does not make the whole mapd directory, the mapd system, or the repository a sunnypilot work.

The notice also describes a five-file IQ.Pilot speed-limit structure. That structure was not present at commit a7892c6, which instead used a consolidated controller and compatibility package files.

The notice states aggregate figures of 39 or more replaced headers and 53 or more mechanically converted files, but it does not publish the complete inventories or a reproducible comparison supporting those totals. Those figures are unverifiable from the notice, and the concrete examples offered to support them are inaccurate for commit a7892c6.

These are not minor discrepancies or competing interpretations of similar code. The notice identifies specific IQ.Pilot files as infringing copies even though those files did not exist in commit a7892c6. A nonexistent file cannot be a 100%-identical copy, cannot contain a replaced copyright header, and cannot support removal of the repository. In particular:

* iqpilot/selfdrive/ui/quiet_mode.py did not exist;  
* iqpilot/livedelay/helpers.py did not exist;  
* iqpilot/livedelay/lagd_toggle.py did not exist;  
* iqpilot/modeld_v2/parse_model_outputs.py did not exist;  
* the alleged public Konn3kt AESCipher/crypto Python source did not exist;  
* the alleged public Konn3kt backup-manager Python source did not exist;  
* neural_network_feed_forward/network.py did not exist;  
* neural_network_feed_forward/nnff.py did not exist;  
* neural_network_feed_forward/locator.py did not exist; and  
* the alleged five-file IQ.Pilot speed-limit structure did not exist.

The __init__.py allegation fails just as plainly. That IQ.Pilot file did exist, but it was a short copyright docstring without sunnypilot's utilities, hashing logic, or enum helpers. It was not the claimed 95%-identical copy. The notice's principal examples therefore alternate between files that were absent and a file whose contents were materially different from what the notice described.

The notice's separate assertion that the repository contained “zero remaining references to sunnypilot” is also false. IQ.Pilot's HKG port includes an express Sunnypilot credit in iqdbc_repo/credits/CREDITS.MD. That credit identifies Sunnypilot, disclaims IQ.Pilot authorship of the credited contribution, and preserves the applicable copyright attribution. The repository therefore did not erase every Sunnypilot reference, did not present every contribution as exclusively IQ.Pilot-authored, and did not support the notice's categorical “zero references” allegation. This is a direct, repository-level contradiction of the notice's attribution-erasure narrative.

3. THE ALLEGED 53-FILE MECHANICAL CONVERSION IS NOT SHOWN BY THE PUBLISHED EXAMPLES.

The notice identifies iqpilot/modeld_v2/parse_model_outputs.py as a 100%-identical namespace replacement. That path did not exist at commit a7892c6. The existing selfdrive/modeld/parse_model_outputs.py closely tracked comma.ai's stock implementation.

The notice alleges renamed public Konn3kt backup source corresponding to sunnypilot backup-manager and AESCipher files. The alleged IQ.Pilot public Python files and AESCipher class were absent from commit a7892c6. The notice therefore cannot establish its claimed public source-to-source mapping. Although object code can contain copyrightable expression, the notice provides no object-code comparison establishing the source-code transformation it alleges.

The three specifically alleged renamed NNLC files, network.py, nnff.py, and locator.py, were not present. IQ.Pilot's neural-network torque functionality was organized and implemented differently.

The notice likewise identifies model-pipeline helpers and compiler files at IQ.Pilot paths that were not present. IQ.Pilot's separate model-management and inference implementation used a materially different structure.

These absent and obsolete mappings directly undermine the assertion that IQ.Pilot was a current 53-file mechanical namespace conversion.

4. THE THREE IDENTIFIERS DESCRIBED AS UNIQUELY GENERATED BY SUNNYPILOT CAME FROM COMMA.AI.

The notice asserts that IQCarParams, IQState, and IQBackupManager used the same identifiers as sunnypilot's CarParamsSP, SelfdriveStateSP, and BackupManagerSP. At commit a7892c6, they instead used:

IQCarParams @0xd4189b5c8aca9f78  
IQState @0xfb0932cf1bde8c5a  
IQBackupManager @0x9f371a75483cf0a3

The notice's quoted values, @0x80ae746ee2596b11, @0x81c2f05a394cf4af, and @0xf98d843bfd7004a3, were not assigned to those IQ.Pilot structs at commit a7892c6.

More fundamentally, all three quoted values appeared in comma.ai's official custom.capnp as CustomReserved identifiers. Comma.ai's file expressly states that the empty structures are reserved for custom forks and that a fork may rename the struct but should not change the identifier:

https://github.com/commaai/openpilot/blob/f870a968e98ceef5740ca99800421a8792e731e0/cereal/custom.capnp

Specifically:

@0x81c2f05a394cf4af is comma.ai CustomReserved0;  
@0x80ae746ee2596b11 is comma.ai CustomReserved4; and  
@0xf98d843bfd7004a3 is comma.ai CustomReserved6.

The notice's statement that these raw identifiers were uniquely generated by sunnypilot is therefore factually incorrect. Even if an earlier fork revision used them, that fact could reflect use of comma.ai's reserved interoperability slots; it would not establish sunnypilot authorship of the identifiers or ownership of the entire surrounding schema.

5. STOCK OPENPILOT AND IQ.PILOT MODIFICATIONS MUST BE ANALYZED SEPARATELY.

selfdrive/modeld/parse_model_outputs.py and selfdrive/car/car_specific.py at commit a7892c6 remained very close to comma.ai's stock implementations. Sunnypilot's inclusion of the same upstream code does not make those files wholly sunnypilot-owned.

IQ.Pilot's root longitudinal planner has a stock openpilot base but also contains materially different IQ.Pilot implementation. The claimant cannot treat an entire file containing substantial preexisting openpilot expression and different IQ.Pilot modifications as wholly claimant-authored without isolating its own protectable contribution.

The same filtration is required for general model runners, event categories, vehicle-control interfaces, planner hooks, torque extensions, live-delay mechanisms, and speed-limit functionality. Similar objectives or interface-compatible behavior in forks of the same upstream project do not by themselves prove copying of claimant-owned expression.

6. THE NOTICE CONFLATES LIMITED INTEGRATION-WRAPPER ALLEGATIONS WITH THE THIRD-PARTY MAPD SYSTEM.

The underlying mapd engine is Jacob Pfeifer's separate project:

https://github.com/pfeiferj/mapd

That project identifies itself as MIT-licensed software intended for integration into openpilot forks. Its documentation describes required schema, service, process, planning, and user-interface integration. The claimant therefore cannot own the mapd engine, the published integration interface, or functional requirements merely because sunnypilot also integrates mapd.

The notice points to similarity in certain Python installer, updater, and short bridge passages. Textual similarity alone does not establish actionable infringement. It does not establish that the claimant authored the matched expression, that the expression is protectable rather than functional or interface-constrained, that IQ.Pilot obtained it from sunnypilot, or that its use was unauthorized. Copyright has no automatic safe or infringing percentage threshold; those questions must be decided passage by passage.

Even assuming solely for argument that a particular integration passage required attribution, permission, replacement, or removal, that would present a limited file- or passage-specific issue. It would not give the claimant ownership of Jacob Pfeifer's engine, IQ.Pilot's independently developed map and tile management, the stock openpilot repository, or the entire IQ.Pilot distribution.

7. THE UI AND OTHER FUNCTIONALITY ALLEGATIONS REQUIRE PASSAGE-SPECIFIC PROOF.

IQ.Pilot's UI was materially different as a whole. Any localized similarity in portions of one settings file must be analyzed at the passage level. It does not establish that the claimant owns IQ.Pilot's entire UI or repository.

Likewise, sharing an ADAS objective, such as dynamic experimental control, neural-network torque control, quiet operation, model selection, speed-limit assistance, or event handling, is not the same as copying source-code expression. The notice must identify the allegedly copied implementation that actually existed in the disabled material and distinguish it from upstream, functional, interface-constrained, third-party, and independently authored elements.

8. LICENSE COMPLIANCE IS FILE-, PASSAGE-, AND VERSION-SPECIFIC.

The presence of a different IQ.Pilot project-level license does not establish that every file is derived from sunnypilot or governed by sunnypilot's current custom license. The relevant questions are whether a particular retained passage was derived from protectable sunnypilot expression, who owned that expression, when and from which version it was obtained, which license governed that version, and whether the applicable conditions were satisfied.

IQ.Pilot retained comma.ai's MIT notice for upstream openpilot. Jacob Pfeifer's mapd has its own MIT license. Those licenses and copyrights cannot be displaced by sunnypilot's project-level license.

Sunnypilot's license history also changed over time. Public history shows a mapd-installer predecessor at commit 0f6b24d035465220be978c5948607bde323a3ee0 on July 2, 2024, while the custom LICENSE.md was added later at commit 7e31333b36066235801a077d9a39e5d1a5614635 on July 29, 2024. Later changes require separate analysis, and this history cannot automatically be applied to the updater or any other later file. The point is not that every disputed passage was necessarily MIT-licensed; it is that the notice cannot apply one later license indiscriminately without identifying the source version and claimant-owned expression at issue.

Any genuinely applicable copyright or permission notice should be preserved for code actually governed by it. But requiring sunnypilot's custom license to replace IQ.Pilot's project license across the entire repository would improperly assert control over comma.ai, third-party, and IQ.Pilot-authored material the claimant does not own.

9. REPOSITORY METADATA AND PRIVATE MESSAGES DO NOT REPLACE A SOURCE COMPARISON.

The notice emphasizes that IQ.Pilot was maintained as a separate GitHub repository and alleges that distribution history was squashed. Neither GitHub's fork button nor a particular commit-history presentation determines copyright ownership. Those facts cannot convert comma.ai, third-party, or IQ.Pilot-authored expression into sunnypilot property.

The notice also quotes private messages concerning "your code," references, and how the project should be described. Those excerpts may provide context or evidence of access, but they do not identify which lines remained in the relevant public tree, who authored those lines, whether they were protectable, or which license governed them. Messages cannot make an absent file present, make comma.ai code claimant-owned, or establish infringement of a particular passage without the necessary source and ownership analysis.

10. LIMITED TEXTUAL SIMILARITY DOES NOT ESTABLISH REPOSITORY-WIDE INFRINGEMENT.

To establish infringement, the claimant must identify expression it owns, separate that expression from upstream and unprotectable material, and show unauthorized copying of the protected expression. The notice does not perform that analysis for the repository as a whole. Its aggregate percentages mix stock openpilot code, third-party material, functional interfaces, absent paths, and materially different implementations.

Even if a passage-specific issue were established, it would not validate the notice's designation of the entire repository as infringing. The proper scope of any remedy must correspond to the claimant-owned expression actually proven to be at issue.

CONCLUSION

The published notice designated the entire repository as infringing based on a mixture of files that did not exist, a package-initializer file that was not remotely the claimed 95%-identical copy, inaccurate source-to-source mappings, stock comma.ai code, identifiers originating in comma.ai's reserved-fork schema, the independently owned mapd system, functional and interface elements, unsupported aggregate figures, and isolated allegations concerning individual passages. Its categorical allegation that IQ.Pilot removed every Sunnypilot reference is independently disproved by the express Sunnypilot attribution in the HKG port's iqdbc_repo/credits/CREDITS.MD.

IQ.Pilot contains extensive stock, third-party, and IQ.Pilot-authored material that SUNNYPILOT LLC does not own. The notice's repository-wide designation did not distinguish that material from any specific claimant-owned expression. Because GitHub's policy treats an entire-repository allegation differently and disables the whole repository, that overbroad identification caused nonclaimant-owned material to be disabled.

For these reasons, I have a good-faith belief that the entire https://github.com/IQLvbs/openpilot repository was removed or disabled as the result of a mistake or misidentification of the material to be removed or disabled.

**I swear, under penalty of perjury, that I have a good-faith belief that the material was removed or disabled as a result of a mistake or misidentification of the material to be removed or disabled.**

**I consent to the jurisdiction of Federal District Court for the judicial district in which my address is located (if in the United States, otherwise the Northern District of California where GitHub is located), and I will accept service of process from the person who provided the DMCA notification or an agent of such person.**

**Please confirm that you have you have read our <a href="https://docs.github.com/articles/guide-to-submitting-a-dmca-counter-notice">Guide to Submitting a DMCA Counter Notice</a>.**

**So that the complaining party can get back to you, please provide both your telephone number and physical address.**

[private]

[private]

**Please type your full legal name below to sign this request.**

[private]
