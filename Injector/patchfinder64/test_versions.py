#!/usr/bin/python
import os, subprocess

for subdir, dirs, files in os.walk("kernel_caches"):
    for version in dirs:
        print("Testing version: {}".format(version));
        d = os.path.join("kernel_caches", version)
        for subdir, dirs, files in os.walk(d):
            for f in files:
                if f.startswith("kernelcache"):
                    out = subprocess.check_output([os.path.join(".", "bin", "patchfinder64"), os.path.join(d, f)]).strip().split("\n")
                    fails = []
                    for test in out:
                        components = test.split(" - ")
                        if components[-1] == "FAIL":
                            fails.append(test)
                    if len(fails) > 0:
                        print("{}/{} - FAILED".format(version, f))
                        print("\t" + "\n\t".join(fails))
                    else:
                        print("{}/{} - PASSED".format(version, f))
        print("")
