from conans import ConanFile, CMake


CONFIGURE_OPTIONS = (
    "program_options"
)


class TCPSend(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    requires = "boost/1.72.0", "pcapplusplus/20.08@bincrafters/stable"
    generators = "cmake"
    default_options = {"boost:shared": False}

    def build(self):
        build_type = self.settings.get_safe("build_type", default="Release")
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
