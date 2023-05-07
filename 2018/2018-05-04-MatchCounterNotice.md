Match Group, LLC:

I am the owner of the intellectual property called "Pofapi".

This letter is a formal response to a DMCA filing, alleging that my
software, Pofapi, is designed to circumvent technological measures that
effectively control access to at least one of Match Group, LLC's
copyright-protected works, and alleges that the following files
fit this criteria:

- github.com/cmpunches/Pofapi/blob/master/pofapi/POFSession.py  
- github.com/cmpunches/Pofapi/blob/master/BroadcastExample.py  
- github.com/cmpunches/Pofapi/blob/master/PhotoSpread.py

I believe the claims of copyright infringement are inaccurate and should
be rejected because:

The complainant misrepresents the basic functionality of Pofapi for fundamental premises of its complaint.

The complainant misrepresents the functionality of standard browser features as access
control mechanisms.

The complainant misrepresents the content that Pofapi accesses.

The complainant misrepresents.

The complaint reads: "This content is designed to circumvent Match
Group’s technological protection measures by forging user-agent and
referrer HTTP headers to misrepresent the source of the HTTP requests.
This content allows users to bypass the official POF network and
improperly access the POF backend system to extract proprietary
information from Match Group’s servers".

This is a patently false representation of the way Pofapi accesses its
content -- as can be plainly seen by even a basic code analysis of any
version of Pofapi ever commited to that repository, Pofapi has not, nor
has it ever "bypassed the POF network", nor does it "improperly access
their backend". Furthermore, nor does it access proprietary or
restricted information not available in any other browser. Furthmore,
nor does it bypass access control mechanisms of any kind.

The Pofapi browser library does not bypass any technological measure
that controls access to *any* content of their site and does not access
any information from their network not otherwise available to other
browsers or browser libraries not otherwise available to any user using
it, and does not access any of that content using means not used by any
other browser.

In fact, Pofapi interfaces compliantly with all technological measures
to control access on POF's site in accordance with global web standards
over HTTP/HTTPS, connecting to the same endpoints, accessing the same
content, and viewing the same data as any other browser using the same
transport and mechanisms as all other web browsers connecting to their
site, allowing access to no more information than mainstream browsers
and the libraries that drive them, with substantially less features than
mainstream browsers and browser libraries: It is, in fact, an atomic
example of exactly what any other browser does: It accesses the HTTP
endpoint, downloads the content in HTML and Javascript using a client
(browser) library, and then renders that content to the user after
parsing it with an engine -- as any reasonable website author would
expect their site to be consumed by literally any standards-compliant
browser currently on the market, including Firefox, Google Chrome,
Internet Explorer and Opera.

In continuance of this, if it is Match Group's contention that every
browser on the market violates their copyright by the nature of the
open standards that drive them and those browsers' architectural designs
and procotol usages, then the claim can not have been filed in good
faith as a website can not be designed in such a way that the only means
of users accessing them, which predate their existence by decades,
violates their copyright or other intellectual property and still be
designed in good faith. I defer the complainant to Title 17 USC, SS
512(f) for clarification on penalties for filing frivolous DMCA
takedowns notices without merit.

Furthermore, while Match Group maintains a private API for its POF
service, and licenses that API's copyrighted material to authorized
users pursuant to a limited license contained in the POF TOS, Pofapi
does not access that private API and does not access any content not
accessed by every other web browser in existence.

While the complainant states that they employ log auditing as a
technological measure of access control, it is, in fact, not a means
of access control. An access control does not meet that definition, and
the user-agent header can not be 'bypassed to gain unauthorized access'.

The widely understood purpose of a user-agent header is for content
negotiation for browser compatibility and is neither bypassed nor is an
accesss control mechanism -- by the nature of its design it is not able
to be /used/ as one either, and is not in the case of POFapi.

The usage of a user-agent header on the server side is so that content
optimized for a specific browser can be served. It is not able to serve
as an access control mechanism, and is in no way imaginable a security
feature or a "technological mechanism to effectively control
access" to any feature on any website by the nature of what it is. To
say otherwise, or to attempt to use it in this fashion, would be a
misunderstanding of the basic technologies in use.

Furthermore, the ability to determine the source of requests or collect
data from users about what browser they are using is not a
copyrightable ability and the data string used for it is entirely at
the discretion of users to share by the nature of what it is and where
it is. Nor is it an access control measure.

Using a user-agent string in Pofapi that is consistent with the browsers
the Pofapi developers ensures consistency between development and
testing. The content served to our browsers is the same content served
to Pofapi. That can't even be construed as "bypassing a technological
mechanism to effectively control access" and strains the definition of
the concept of "good faith" when paired with even an introductory
understanding of how these tools and components work.

Furthermore, the complainant states that they employ referrer header
checks in post-connection log audits as a means of access control, which
also does not meet the definition of a technological measure to control
access and is not a thing that can be 'bypassed to gain unauthorized
access' by the nature of what the referrer header is and how the
complainant has already stated that they use it.

A referrer check is to prevent cross-site forgery requests, which Pofapi
does not and can not make to POF servers by the nature of its design. It
simply is not able to serve that function in any design.

The complainant can not reasonably construe our use of a
referrer header in consistency with all other browsers as bypassing an
access control mechanism without changing either the definition of the
referrer header or changing the definition of an 'access control
mechanism'.

The user's choice of web browser to access a website is not a
copyrightable intellectual property.

Pofapi uses standard web browser components internally, to access
the same material a GUI browser accesses, using the same
mechanisms on their site as any other standards based browser, including
the use of a referrer url, which is a feature all web browsers employ.

It accesses absolutely no content not otherwise available to the user in
any other web browser.

I must emphasize to the complainant that a DMCA notice is drafted under
penalty of perjury and that simply saying you are making it in good
faith is not the only criteria for determining good faith-- as such it
would be well advised not to file further frivolous legal notices to
cause annoyance, disruption, damages.

In furtherance of that, please be aware that continued malfeasance
could result in legally protected development of new
features, as well as co-location of the source code -- by someone that
clearly eclipses even the apex points of software engineering and
architecture design at your client's organizations. I strongly
encourage you to research both this issue and your correspondent
further before deciding that a response is appropriate.

I have received no offer for purchase of my intellectual property from Match Group. Surely that would be a more appropriate and productive talk.

I have read and understand GitHub's Guide to Filing a DMCA Counter
Notice.

This communication to you is a DMCA counter notification letter as
defined in 17 USC 512(g)(3):

I declare, under penalty of perjury, that I have a good faith belief
that the complaint of copyright violation is based on mistaken
information, misidentification of the material in question, or
deliberate misreading of the law.

I ask that Github, upon receipt of this counter-notification, restore
the material in dispute, unless the complainant files suit against me
within ten (10) days, pursuant to 17 USC 512(g)(2)(B).

My name, address, and telephone number are:

[private]

I hereby consent to the jurisdiction of Federal District Court for the
judicial district in which I reside.

I agree to accept service of process from the complainant.

No content identified by the complainant is infringing of their
copyrighted works, bypasses any technological measure of access control,
and all content referred to consists entirely of my own copyrighted and
licensed work except where expressly stated in the source code-- source
code which easily verifies every point in this counternotice. I did
expect github to protect me and my projects from frivolous complaints of
this nature, so, I'm disappointed, but hopefully this clarification resolve the
issue.

Best regards,  
[private]
