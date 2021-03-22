# patchfinder64

Based of xerub's patchfinder64, adds various patches which should work across a variety of versions.

Patches written by myself are included here. These include patches from async_wake-fun and from unc0ver

## Testing

This fork includes a testing portion, allowing us to confirm that the results returned from the patchfinder are valid.

To run the tests, you must do the following:
1. Run `./fetch_kerncaches`, or run `python ./fetch_kerncaches`. **Please note this will download 13 iOS versions**, and extract and decompress the kernelcaches. You can change which versions of iOS are to be downloaded in the file.
2. Run `make patchfinder64`
3. Run `python test_versions.py`

This will return what version have their patchfinders working correctly, and will tell you which functions are not finding the correct values (PF = Patch Finder, AS = Actual Symbol).
