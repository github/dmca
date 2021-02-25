from conans import ConanFile
import os
import shutil


class Ps2devCMakeToolchainConan(ConanFile):
    name = "ps2dev-cmaketoolchain"
    description = "CMake toolchain script for ps2dev"
    topics = "ps2", "sdk", "library", "sony", "playstation", "ps2"

    def export_sources(self):
        self.copy("*.cmake*", dst="cmake")
        self.copy("Platform", dst="cmake")

    def package(self):
        shutil.copytree(os.path.join(self.source_folder, "cmake"),
                        os.path.join(self.package_folder, "cmake"))

    def package_info(self):
        self.user_info.cmake_dir = os.path.join(self.package_folder, "cmake").replace("\\", "/")
        self.user_info.cmake_toolchain_file = os.path.join(self.package_folder, "cmake", "cmaketoolchain.cmake").replace("\\", "/")
