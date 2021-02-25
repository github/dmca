# Release checklist

## Source release

- Update OP_LT_* API versioning in configure.ac.
- Check for uncommitted changes to master.
- Prepare win32 binaries
  - Do this before tagging the release, as it may require changes which should
    be committed
- Tag the release commit with 'git tag -s vN.M'.
  - Include release notes in the tag annotation.
- Verify 'make distcheck' produces a tarball with
  the desired name.
- Push tag to public repo.
- Upload source package 'opusfile-${version}.tar.gz'
  to website and verify file permissions.
- Update checksum files on website.
- Update links on <https://www.opus-codec.org/downloads/>.
- Add a copy of the documentation to <https://www.opus-codec.org/docs/>
  and update the links.
  - Add doc/latex/refman as docs/opusfile_api-${version}.pdf on opus-codec.org
  - Add doc/html as docs/opusfile_api-${version} on opus-codec.org

Releases are commited to https://svn.xiph.org/releases/opus/
which propagates to downloads.xiph.org, and copied manually
to https://archive.mozilla.org/pub/opus/

Release notes and package links should be added to the corresponding
tag at https://gitlab.xiph.org/xiph/opusfile so they show on the
releases page.

Release packages should also be manually attached to the corresponding
tag on the github mirror https://github.com/xiph/opusfile/releases

## Win32 binaries

- Install cross-i686-w64-mingw32-gcc and associated binutils.
  - If you skip this step, libopus will still try to build with the system gcc
    and then fail to link.
- Edit mingw/Makefile to point to the latest versions of libogg. opus, openssl
  (see <https://archive.mozilla.org/pub/opus/>, checksums in SHA256SUMS.txt)
- run `make -C mingw`
  - Downloads versions of libogg, opus, openssl.
  - Compiles them.
  - Compiles static opusfile and examples against the built deps.
- run `make -C mingw package`
  - Creates an opusfile-${version}-win32.zip binary package.
- Merge changes between README.md and the version in the last
  binary release. E.g. it's good to include versions of the dependencies,
  release notes, etc.
- Copy the archive to a clean system and verify the examples work
  to make sure you've included all the necessary libraries.
- Upload the archive zipfile to websites.
- Verify file permissions and that it's available at the expected URL.
- Update links on <https://www.opus-codec.org/downloads/>.

Binary releases are copied manually to s3 to appear at
https://archive.mozilla.org/pub/mozilla.org/opus/win32/
