from conans import AutoToolsBuildEnvironment, ConanFile, tools
import os

class OathToolkitConan(ConanFile):
    name = "oath-toolkit"
    license = "LGPLv2+ (libraries) and GPLv3+ (tools)"
    description = "OATH Toolkit provide components to build one-time password authentication systems."
    homepage = "https://www.nongnu.org/oath-toolkit/"
    topics = ("authentication", "OTP")
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False]}
    default_options = {"shared": False, "fPIC": True}
    _autotools = None

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    @property
    def _source_subfolder(self):
        return "source_subfolder"

    def source(self):
        tools.get(**self.conan_data["sources"][self.version])
        extracted_dir = "oath-toolkit-" + self.version
        os.rename(extracted_dir, self._source_subfolder)

    def _configure_autotools(self):
        if self._autotools is not None:
            return self._autotools
        self._autotools = AutoToolsBuildEnvironment(
            self, win_bash=tools.os_info.is_windows
        )
        configure_args = []
        configure_file_path = os.path.join(self._source_subfolder)
        self._autotools.configure(
            configure_dir=configure_file_path, args=configure_args
        )
        return self._autotools

    def build(self):
        autotools = self._configure_autotools()
        autotools.make()

    def package(self):
        self.copy(pattern="COPYING", src=self._source_subfolder, dst="licenses")
        autotools = self._configure_autotools()
        autotools.install()

    def package_info(self):
        self.cpp_info.components["oath"].names["pkg_config"] = "liboath"
        self.cpp_info.components["pskc"].names["pkg_config"] = "libpskc"
        self.cpp_info.components["oath"].libs = ["oath"]
        self.cpp_info.components["pskc"].libs = ["pskc"]
        self.cpp_info.components["oath"].includedirs = ["include/liboath"]
        self.cpp_info.components["pskc"].includedits = ["include/pskc"]
        bindir = os.path.join(self.package_folder, "bin")
        self.output.info("Appending PATH environment variable: {}".format(bindir))
        self.env_info.PATH.append(bindir)
