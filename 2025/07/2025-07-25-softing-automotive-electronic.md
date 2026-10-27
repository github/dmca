Before disabling any content in relation to this takedown notice, GitHub
- contacted the owners of the affected repositories to give them an opportunity to [make changes](https://docs.github.com/en/github/site-policy/dmca-takedown-policy#a-how-does-this-actually-work).
- provided information on how to [submit a DMCA Counter Notice](https://docs.github.com/en/articles/guide-to-submitting-a-dmca-counter-notice).

To learn about when and why GitHub may process some notices this way, please visit our [README](https://github.com/github/dmca/blob/master/README.md#anatomy-of-a-takedown-notice).

---

Dear Sir or Madam:
 
We hereby inform you that Softing Automotive Electronic GmbH, [private] (hereinafter referred to as the “Client”) has instructed us to contact you regarding a copyright infringing software made available on your platform. Please find attached the according
power of attorney as [private].
 
We had already contacted you regarding another “takedown” down notice on April 14, 2025 regarding the same code, which was resolved just recently.
 
The reason for this notice-and-take-down letter is that our Client found software offered on your platform which seems to be identical to the one you put
offline after our last notice-and-take-down request. This software also enables an unauthorized circumvention of our Client's copyright protection systems. This constitutes a violation of Sec. 95a (1) of the [private] Copyright Act and a violation of Article 1201 of the Digital Millenium Copyright Act  
(hereinafter referred to as “DMCA”).
 
I. Software Automotive Electronic and its software
 
[1] Our Client specializes in hardware and software solutions for automotive electronics, focusing on diagnostics, communication and test automation throughout the entire lifecycle of control units – from development to customer service. We enclose an extract from the German commercial register as [private].
 
[2] Our Client’s products include the software Softing Smart Diagnostic Engine (hereinafter referred to as “SDE”) of which she is the rights holder. This letter is about the circumvention of the technical protection of this software; information about the SDE software can be found at [private].
 
[3] SDE is a platform-independent, high-performance runtime system designed for executing diagnostic functions, sequences, and services throughout the entire vehicle lifecycle. It supports standardized diagnostic formats like ODX and OTX, and offers a functional API that facilitates both simple and complex diagnostic tasks without requiring deep diagnostic expertise. SDE is versatile, supporting various applications including engineering, testing, manufacturing, aftersales, in-vehicle diagnostics, and cloud-based solutions.
 
[4] The software layer SDE is used by many of our Client’s OEM customers ("Original Equipment Manufacturers"). OEM products are typically resold under the OEM’s own brand name. OEM customers do not use the products themselves but distribute them as part of a larger offering to end users.
 
[5] To be able to communicate to the vehicles, the software layer SDE requires a description for each control unit of how communication with this control unit works. In our Client’s case, this is the socalled SMR data. SMR data stands for “Service, Maintenance, and Repair data”. It includes essential technical information such as diagnostic trouble codes, repair instructions, maintenance schedules, and parts lists used by workshops to service and repair vehicles. Access to SMR data is crucial for ensuring fair competition in the automotive aftermarket and is regulated under EU law, particularly Regulation (EU) 2018/858.
 
[6] This SMR data is created by our Client’s customers (i.e. car manufacturers) and contain customer secrets. That is why this data is also encrypted. This data is distributed worldwide to all of the OEM customers' workshops and production sites. With this protected information, it is possible, at least for older vehicles, to unlock and manipulate the control units.
 
[7] This bypasses the protective measures taken by the OEMs. Newer vehicle generations contain better protective measures and cannot be manipulated so easily with this data. But even in this case, one can gain insights into how an OEM's control unit functions, which facilitates attacks.
 
[8] Our Client's SMR software is protected as a copyrighted work and as a computer program in accordance with Sect. 2 para. 1 no. 1, 69a et seq. of the [private] Copyright Act as well asbArt. 101 (a), 102 of the 17 U.S. Copyright Act. Our Client's software is not licensed under an open source license.
 
II. Circumvention of copyright protection systems
 
[9] On your company's website, available at <github.com>, software is made available that can be used to decrypt our Client’s SMR data. Decryption means that anyone with access to this SMR data can access the secrets of our Client's customers.
 
[10] The technical protection measures used by our Clients to encrypt the data work as follows: The relevant data is secured by a Blowfish encryption algorithm. The data is decrypted by an application that has a user licensing mechanism. The user does not see the data, but uses it to communicate with the control unit. The data is encrypted at all times and is only loaded into memory and used by the control unit's unlocking mechanism for a short period.
 
[11] The software available at your platform can be used to circumvent the technical protection measure of our Client’s software through encryption. We submit as [private] screenshots of the infringing repository.
 
[12] The software refers to our Client's software by mentioning so-called “SMR files”. This file format was defined by our client and is used worldwide in the automotive sector by various customers. SMR files are a well-known format in the field. Therefore, their explicit mention in the repository clearly indicates the circumvention of the copyright protection systems of our client's software.
 
[13] Furthermore, an explicit reference to our client's software and its encryption can be found in the software file “BlowFishTable.rs” of the infringing repository. This file contains our client's private key [private] ([private]).
 
[14] The software is identical to the one we had flagged to you in April: it extracts the hidden private keys from the C++ libraries and uses them to remove the BLOWFISH encryption. The intention is clearly written to gain illegal access to the secrets hidden in the JAR files, which are contained in the encrypted data. The exact steps including reverse engineering, the XOR transformation, the Blowfish decryption and inflation to circumvent our Client’s protection mechanism is contained in the project description.
 
[15] The software that is accessible from your company's website allows the technical protection measures of our Client's software to be circumvented. For this reason, it is an illegal circumvention of technical protection measures under Sec. 95a (1) of the German Copyright Act as well as a violation of Art. 1201 of the DMCA.
 
III. Person of the infringer unknown
 
[16] Our Client does not have the contact details of the persons responsible for the software that enables such unauthorized circumvention of the copyright protection systems. However, according to the information on your company's website, the user with the GitHub name “@rnd-ash” seem to be responsible (similar to the last incident we had reported, see [private])
 
IV. GitHub’s responsibility and liability
 
[17] The infringing software is, as previously shown, available to everyone via your company's website and is only made publicly accessible through it.
 
[18] As a software development platform and content host, your company qualifies as an online service provider under the DMCA. Once your company receives a valid DMCA takedown notice alleging the unauthorized use of copyrighted material, it is legally obligated to act expeditiously to remove or disable access to the infringing content to maintain its safe harbor protection under 17 U.S.C. § 512(c). This notice-and-takedown framework ensures that intermediaries like your
company play a key role in preventing the continued dissemination of infringing content while balancing the rights of content creators and users.
 
[19] We request to immediately delete the reported content, that is to say, the whole of the entire repository available at <https://github.com/rnd-ash/OpenVehicleDiag/tree/main/SMRParser> [private]), at the latest by July 17, 2025 (CET)
 
[20] The deadline is required to be set on short notice because the available software on your company’s website enables third parties to bypass technical protection measures and gain unauthorized access to encrypted data used to communicate with vehicle control units. As pointed out, the availability of this circumvention tool allows manipulation of control units, particularly in older vehicle models, and enables reverse engineering of newer systems, which poses a serious risk to the integrity of OEM security measures and vehicle safety.
 
[21] We expect to receive a confirmation that the entire repository has been deleted.
 
[22] Please note that if you do not take the appropriate steps, you may be held directly liable for the circumvention of copyright protection systems. If the deadline expires without you having complied with our requests, we will recommend that our Client immediately asserts the above claims against your company.
 
[23] Finally and in accordance with the DMCA guidelines on your company's website, we hereby make
the following statements:
 
I have a good faith belief that use of the copyrighted materials described above on the infringing web pages is not authorized by the copyright owner, or its agent, or the law. I have taken fair use into consideration.
 
I swear, under penalty of perjury, that the information in this notification is accurate and that I am authorized to act on behalf of the owner of an exclusive right that is allegedly infringed.
 
[24] For the avoidance of doubt: this letter is not intended to and does not waive any of our Client’s legal rights and remedies, all of which are hereby expressly reserved. Furthermore, this letter does not constitute any legal obligation of our Client vis-à-vis you, whether under contract, law, or otherwise.
 
Yours sincerely
 
[SIGNATURE]
 
[private] [private]
[private] / Attorney-at-Law [private] / Attorney-at-Law  
[private]  
[private]  
T [private] | F [private]  
