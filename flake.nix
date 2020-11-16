{
  description = "Domestic Supervisor";
  inputs = {
    nixpkgs.url = "nixpkgs";
    wt = {
      url = "github:emweb/wt/4.4.0";
      flake = false;
    };
    dlib = {
      url = "http://dlib.net/files/dlib-19.21.tar.bz2";
      flake = false;
    };
    opencv = {
      url = "https://github.com/opencv/opencv/archive/4.5.0.zip";
      flake = false;
    };
    paho-mqtt-cpp = {
      url = "https://github.com/eclipse/paho.mqtt.cpp/archive/v1.1.tar.gz";
      flake = false;
    };
    paho-mqtt-c = {
      url =
        "https://github.com/eclipse/paho.mqtt.c/releases/download/v1.3.6/Eclipse-Paho-MQTT-C-1.3.6-Linux.tar.gz";
      flake = false;
    };
    spdlog = {
      url = "https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz";
      flake = false;
    };
  };
  outputs = { self, nixpkgs, ... }@inputs:
    let
      pkgs = import nixpkgs {
        system = "x86_64-linux";
        config.allowUnfree = true;
      };
      stdenvClang = with pkgs; overrideCC stdenv llvmPackages_11.libcxxClang;
      clang-tools = with pkgs;
        pkgs.clang-tools.override { llvmPackages = llvmPackages_11; };

      boostClang =
        (pkgs.boost174.override { stdenv = stdenvClang; }).overrideAttrs (old: {
          LDFLAGS = "-fuse-ld=lld";
          AR = "llvm-ar";
          RANLIB = "llvm-ranlib";
          nativeBuildInputs = old.nativeBuildInputs
            ++ [ pkgs.llvmPackages_11.bintools ];
        });
      wt = stdenvClang.mkDerivation {
        name = "wt";

        enableParalellBuilding = true;

        src = inputs.wt;

        nativeBuildInputs = with pkgs; [ cmake llvmPackages_11.bintools ];
        buildInputs = with pkgs; [ boostClang pango graphicsmagick libGLU zlib ];
        cmakeFlags = [
          "--no-warn-unused-cli"
          "-DWT_CPP14_MODE=-std=c++14"
          "-DWT_WRASTERIMAGE_IMPLEMENTATION=GraphicsMagick"
          "-DGM_PREFIX=${pkgs.graphicsmagick}"
          "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld"
        ];

        AR = "llvm-ar";
        RANLIB = "llvm-ranlib";
      };
      dlib = stdenvClang.mkDerivation {
        name = "dlib";

        enableParallelBuilding = true;

        src = inputs.dlib;

        nativeBuildInputs = with pkgs; [ cmake pkgs.llvmPackages_11.bintools ];
        buildInputs = with pkgs; [
          cudaPackages.cudatoolkit_11
          cudnn_cudatoolkit_11
          openblas
          lapack
        ];
        cmakeFlags = [
          "--no-warn-unused-cli"
          "-DUSE_AVX_INSTRUCTIONS=1"
          "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld"
          "-DDLIBUSECUDA=1"
        ];

        AR = "llvm-ar";
        RANLIB = "llvm-ranlib";
      };
      opencv = stdenvClang.mkDerivation {
        name = "opencv";

        enableParallelBuilding = true;

        src = inputs.opencv;

        nativeBuildInputs = with pkgs; [ cmake pkgs.llvmPackages_11.bintools ];
        buildInputs = with pkgs; [
          ffmpeg_4
          gst_all_1.gstreamer
          gst_all_1.gst-plugins-good
        ];
        cmakeFlags = [
          "--no-warn-unused-cli"
          "-DCPU_BASELINE=AVX"
          "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld"
        ];

        AR = "llvm-ar";
        RANLIB = "llvm-ranlib";
      };
      paho-mqtt-c = stdenvClang.mkDerivation {
        name = "paho-mqtt-c";

        src = inputs.paho-mqtt-c;

        dontBuild = true;

        installPhase = "cp -r . $out";
      };
      paho-mqtt-cpp = stdenvClang.mkDerivation {
        name = "paho-mqtt-cpp";

        src = inputs.paho-mqtt-cpp;

        nativeBuildInputs = with pkgs; [ cmake pkgs.llvmPackages_11.bintools ];
        buildInputs = [ paho-mqtt-c ];
        cmakeFlags = [
          "--no-warn-unused-cli"
          "-DPAHO_WITH_SSL=0"
          "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld"
        ];

        AR = "llvm-ar";
        RANLIB = "llvm-ranlib";
      };
      spdlog = stdenvClang.mkDerivation {
        name = "spdlog";

        src = inputs.spdlog;

        nativeBuildInputs = with pkgs; [ cmake pkgs.llvmPackages_11.bintools ];
        cmakeFlags =
          [ "--no-warn-unused-cli" "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld" ];

        AR = "llvm-ar";
        RANLIB = "llvm-ranlib";
      };
      domestic-supervisor = isShell:
        stdenvClang.mkDerivation rec {
          name = "domestic-supervisor";

          src = self;

          nativeBuildInputs = [
            pkgs.cmake
            pkgs.ninja
            pkgs.llvmPackages_11.libcxx
            pkgs.llvmPackages_11.libcxxabi
            pkgs.llvmPackages_11.bintools
          ] ++ pkgs.lib.optional (!isShell) pkgs.autoPatchelfHook
            ++ pkgs.lib.optionals isShell [ clang-tools pkgs.lldb_11 ];
          buildInputs =
            [ boostClang wt dlib opencv paho-mqtt-cpp paho-mqtt-c spdlog pkgs.zlib ];
          cmakeFlags =
            [ "--no-warn-unused-cli" "-DCMAKE_EXE_LINKER_FLAGS=-fuse-ld=lld" ];

          AR = "llvm-ar";
          RANLIB = "llvm-ranlib";
          LD_LIBRARY_PATH = if isShell then
            pkgs.lib.makeLibraryPath (buildInputs
              ++ [ pkgs.llvmPackages_11.libcxx pkgs.llvmPackages_11.libcxxabi ])
          else
            null;
          CPLUS_INCLUDE_PATH =
            "${pkgs.llvmPackages_11.libcxx}/include/c++/v1:${pkgs.clang_11.libc_dev}/include:${pkgs.clang_11.cc}/lib/clang/${pkgs.clang_11.version}/include";
          ZLIB_LIBRARY = "${pkgs.zlib}/lib";
          ZLIB_INCLUDE_DIR = "${pkgs.zlib}/include";
        };
    in {
      devShell.x86-64-linux = domestic-supervisor true;
      defaultPackage.x86_64-linux = domestic-supervisor true;
      packages.x86_64-linux = {
        domestic-supervisor = domestic-supervisor false;
        inherit opencv dlib wt paho-mqtt-c paho-mqtt-cpp spdlog;
      };
    };
}
