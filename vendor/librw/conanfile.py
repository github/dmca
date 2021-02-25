from conans import ConanFile, CMake, tools
from conans.errors import ConanException, ConanInvalidConfiguration
import os
import shutil
import textwrap


class LibrwConan(ConanFile):
    name = "librw"
    version = "master"
    license = "MIT"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake", "cmake_find_package"
    options = {
        "platform": ["null", "gl3", "d3d9", "ps2"],
        "gl3_gfxlib": ["glfw", "sdl2"],
    }
    default_options = {
        "platform": "gl3",
        "gl3_gfxlib": "glfw",
        "openal:with_external_libs": False,
        "sdl2:vulkan": False,
        "sdl2:opengl": True,
        "sdl2:sdl2main": True,
    }
    no_copy_source = True

    @property
    def _os_is_playstation2(self):
        try:
            return self.settings.os == "Playstation2"
        except ConanException:
            return False

    def config_options(self):
        if self._os_is_playstation2:
            self.options.platform = "ps2"
        if self.settings.os == "Windows":
            self.options["sdl2"].directx = False

    def configure(self):
        if self.options.platform != "gl3":
            del self.options.gl3_gfxlib

    def validate(self):
        if self.options.platform == "d3d9" and self.settings.os != "Windows":
            raise ConanInvalidConfiguration("d3d9 can only be built for Windows")
        if self.options.platform == "ps2":
            if not self._os_is_playstation2:
                raise ConanInvalidConfiguration("platform=ps2 is only valid for os=Playstation2")
        else:
            if self._os_is_playstation2:
                raise ConanInvalidConfiguration("os=Playstation2 only supports platform=ps2")

    def requirements(self):
        if self.options.platform == "gl3":
            if self.options.gl3_gfxlib == "glfw":
                self.requires("glfw/3.3.2")
            elif self.options.gl3_gfxlib == "sdl2":
                self.requires("sdl2/2.0.12@bincrafters/stable")
        elif self.options.platform == "ps2":
            self.requires("ps2dev-ps2sdk/unknown@madebr/testing")
        if self._os_is_playstation2:
            self.requires("ps2dev-cmaketoolchain/{}".format(self.version))

    def export_sources(self):
        for d in ("cmake", "skeleton", "src", "tools"):
            shutil.copytree(src=d, dst=os.path.join(self.export_sources_folder, d))
        self.copy("args.h")
        self.copy("rw.h")
        self.copy("CMakeLists.txt")
        self.copy("LICENSE")

    @property
    def _librw_platform(self):
        return {
            "null": "NULL",
            "gl3": "GL3",
            "d3d9": "D3D9",
            "ps2": "PS2",
        }[str(self.options.platform)]

    def build(self):
        if self.source_folder == self.build_folder:
            raise Exception("cannot build with source_folder == build_folder")
        if self.options.platform == "gl3" and self.options.gl3_gfxlib == "glfw":
            tools.save("Findglfw3.cmake",
                       textwrap.dedent(
                           """
                           if(NOT TARGET glfw)
                             message(STATUS "Creating glfw TARGET")
                             add_library(glfw INTERFACE IMPORTED)
                             set_target_properties(glfw PROPERTIES
                                INTERFACE_LINK_LIBRARIES CONAN_PKG::glfw)
                           endif()
                           """), append=True)
        tools.save("CMakeLists.txt",
                   textwrap.dedent(
                       """
                       cmake_minimum_required(VERSION 3.0)
                       project(cmake_wrapper)

                       include("{}/conanbuildinfo.cmake")
                       conan_basic_setup(TARGETS)

                       add_subdirectory("{}" librw)
                       """).format(self.install_folder.replace("\\", "/"),
                                   self.source_folder.replace("\\", "/")))
        cmake = CMake(self)
        env = {}
        cmake.definitions["LIBRW_PLATFORM"] = self._librw_platform
        cmake.definitions["LIBRW_INSTALL"] = True
        cmake.definitions["LIBRW_TOOLS"] = True
        if self.options.platform == "gl3":
            cmake.definitions["LIBRW_GL3_GFXLIB"] = str(self.options.gl3_gfxlib).upper()
        if self._os_is_playstation2:
            cmake.definitions["CMAKE_TOOLCHAIN_FILE"] = self.deps_user_info["ps2dev-cmaketoolchain"].cmake_toolchain_file
            env["PS2SDK"] = self.deps_cpp_info["ps2dev-ps2sdk"].rootpath
        with tools.environment_append(env):
            cmake.configure(source_folder=self.build_folder)
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.includedirs.append(os.path.join("include", "librw"))
        self.cpp_info.libs = ["librw" if self.settings.compiler == "Visual Studio" else "rw"]
        if self.options.platform == "null":
            self.cpp_info.defines.append("RW_NULL")
        elif self.options.platform == "gl3":
            self.cpp_info.defines.append("RW_GL3")
            if self.options.gl3_gfxlib == "sdl2":
                self.cpp_info.defines.append("LIBRW_SDL2")
        elif self.options.platform == "d3d9":
            self.cpp_info.defines.append("RW_D3D9")
        elif self.options.platform == "ps2":
            self.cpp_info.defines.append("RW_PS2")
