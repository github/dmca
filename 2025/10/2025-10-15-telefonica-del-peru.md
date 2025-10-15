Section 1: Identification of Parties and Authorization  

1.1. Rights Holder: Telefónica del Perú S.A.A., with its principal place of business in  
[private].  

1.2. Authorized Representative: [private], Authorized  
Representative, Cybersecurity.  

1.3. Statement of Authority: The undersigned declares to be authorized to act on  
behalf of Telefónica del Perú S.A.A. in matters of intellectual property protection. The  
proactive submission of this documentation is intended to unequivocally establish the  
legitimacy of this demand, thereby avoiding procedural delays that may arise from  
requests for additional information.  

Section 2: Executive Summary of Infringing Repositories  
To facilitate an expedited review, the following table summarizes the infringements.  
The structure of this table allows the reviewer to immediately grasp the full scope of  
the issue, demonstrating meticulous preparation that facilitates decision-making.  

| Repository Identifier             | Full Repository URL                                      | Owner (Handle) | Primary GitHub Policy Violations                       | Associated Support Tickets |
|-------------------------|---------------------------------------------------------------------|----------------|--------------------------------------------------------|---------------------------|
| AutoWebMovistarMagneto  | https://github.com/edfernandeza/AutoWebMovistarMagneto              | edfernandeza   | [private] | [private]                  |
| AutoWebMovistarBerserkers| https://github.com/edfernandeza/AutoWebMovistarBerserkers          | edfernandeza   | [private] | [private]                  |

Section 3: Ground 1: Copyright Infringement (Violation of DMCA Policy)  

3.1. Description of the Original Copyrighted Work  
The copyrighted work consists of a collection of internal software systems,  
proprietary development and automation frameworks, business logic, and source  
code developed over several years by the engineering teams of Telefónica del Perú  
S.A.A. and its affiliates. This material is confidential, is not open source, and has never  
been licensed for distribution or publication on public platforms. Its value lies in its  
proprietary nature and its central role in the company’s operations.  

The work includes, but is not limited to:  

• Proprietary automation frameworks built on top of Selenium and Cucumber,  
specifically designed for testing Telefónica’s e-commerce and internal systems  
• Business process implementations encoded in Gherkin feature files that  
describe proprietary workflows for customer service, product sales, migrations,  
and technical operations  
• Internal Java packages using the namespace [private], which is  
exclusive to Telefónica’s development organization  
• Configuration management systems containing environment-specific settings,  
API integrations, and deployment pipelines  
• Proprietary data structures including product catalogs, geographic databases,  
and customer interaction models  

3.2. Analysis of Infringement in AutoWebMovistarMagneto (Ticket [private])  

This repository contains an unauthorized copy of a test automation project for the  
Movistar Peru e-commerce platform. The evidence of infringement, detailed in Annex  
B of Document 3, is overwhelming:  

• The repository contains 1,194 files organized in a complete project structure  
• The pom.xml file declares a dependency on [private], an internal proprietary framework that is not publicly  
available and is hosted on Telefónica’s private [private] server  
• The application.yml file exposes internal environment URLs (e.g.,  
[private]), test credentials (user:  
[private]), and API secrets for the XRAY test management  
platform  
• The cfg/devops.properties file reveals CI/CD pipeline configurations and  
integration with the corporate Artifactory repository  
• The staticData/ directory contains proprietary product catalogs with internal  
SKU codes (e.g., [private]) and geographic data ([private])  
specific to the [private] operation  
• Hundreds of feature files describe detailed business processes for number  
portability, device sales, plan renewals, and home internet services  

3.3. Analysis of Infringement in AutoWebMovistarBerserkers (Ticket  
[private])  

This repository, containing an estimated 48,594 lines of proprietary code, exposes  
another critical internal system. The evidence, detailed in Annex C of Document 3,  
includes:  

• The repository contains 671 files including 158 Java files and 372 Gherkin  
feature files  
• The pom.xml file declares a dependency on [private], the  
same internal proprietary framework  
• Feature files reference the internal application “DITO” (Digital Transformation  
Operations), which is a confidential business system  
• The code implements complex business processes including:  
– CASI (SIM card change processes)  
– CAPL (plan change processes)  
– CAEQ (device change processes)  
– Portabilidad (number portability)  
– Migraciones (service migrations)  
– Paquetización (service bundling)  
• The Java package structure uses [private], confirming its proprietary  
origin  
• The repository includes internal API request templates and business validation  
logic  

3.4. Declaration of Total Infringement  

Based on the evidence presented, it is asserted that the entire content of both  
repositories constitutes an infringing work. There is no separable content that  
does not infringe on Telefónica’s copyright. Therefore, we request the complete and  
immediate removal of both repositories in their entirety, in accordance with  
GitHub’s policy that allows for such action when the infringement covers the entire  
repository.  

Section 4: Ground 2: Trademark Infringement (Violation of Trademark  
Policy)  

4.1. Trademark Ownership  

Telefónica S.A. is the legitimate and exclusive owner of the registered trademark  
“MOVISTAR” in the Republic of Peru. The current Trademark Registration Certificate,  
issued by the National Institute for the Defense of Competition and the Protection of  
Intellectual Property (INDECOPI), is referenced in Annex A of Document 3.  

4.2. Analysis of Infringement  

The prominent and central use of the “Movistar” trademark in the names of both  
repositories (AutoWebMovistarMagneto and AutoWebMovistarBerserkers)  
constitutes a clear violation of GitHub’s Trademark Policy. This use is not nominative  
or referential; on the contrary, it is designed to create a direct association with the  
brand.  

A third party encountering these repositories could mistakenly conclude that they are  
official projects, sanctioned, or endorsed by Movistar or Telefónica. This deliberate or  
negligent confusion: - Damages the brand’s reputation - Dilutes its exclusivity -  
Creates potential liability for Telefónica - Misleads developers and users about the  
origin and authorization of the code  

4.3. Requested Action  

Given that the infringing name is a fundamental component of the repositories’  
identification and URL, the only effective remedy is their complete removal.  

Section 5: Ground 3: Exposure of Private Information (Violation of Private  
Information Removal Policy)  

5.1. Applicable GitHub Policy  

Independent of the copyright and trademark claims, the content of the repositories  
also violates the “GitHub Private Information Removal Policy”. This policy allows  
for the removal of data that is confidential and poses a security risk, even if it is not  
explicitly protected by copyright.  

5.2. Evidence of Exposed Private Information  

The AutoWebMovistarMagneto repository exposes information that represents a  
direct and tangible security risk to Telefónica’s infrastructure:  

• Internal environment URLs: [private],  
[private], [private]  
• Test credentials: Username [private] with associated passwords  
• API authentication tokens: XRAY platform credentials including client ID and  
secret  
• Internal server addresses: Artifactory repository URLs and deployment  
endpoints  
• Proprietary product identifiers: Internal SKU codes that map to specific  
inventory and pricing systems  
• Geographic data structures: Internal ubigeo (geographic location) codes used  
in customer databases  

The AutoWebMovistarBerserkers repository similarly exposes: - Internal business  
process workflows that could be exploited to understand system vulnerabilities - API  
request structures that reveal internal service architectures - Test data that includes  
patterns used in production systems  

This exposure creates multiple security risks: - Attack surface expansion: External  
parties can identify internal systems to target - Credential compromise: Even test  
credentials can provide insights into authentication patterns - Business intelligence  
leakage: Competitors can analyze proprietary processes and pricing structures -  
Social engineering vectors: Information can be used to craft convincing phishing  
attacks  

Section 6: Required Declarations  

In accordance with the requirements of the DMCA and GitHub’s Guide to Submitting  
a DMCA Takedown Notice, the undersigned declares the following:  
“I have a good faith belief that use of the copyrighted materials described  
above on the infringing web pages is not authorized by the copyright owner,  
or its agent, or the law.”  
“I have taken fair use into consideration.”  
“I swear, under penalty of perjury, that the information in this notification is  
accurate and that I am the copyright owner, or am authorized to act on  
behalf of the owner, of an exclusive right that is allegedly infringed.”  

Signature:  
[private]

[private]  
Authorized Representative, Cybersecurity  
Telefónica del Perú S.A.A.  

Contact Information:    
Email: [private]  
Phone: [private]

Date: [private]
