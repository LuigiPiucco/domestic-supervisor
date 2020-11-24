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
      url = "https://github.com/eclipse/paho.mqtt.c/archive/v1.3.6.tar.gz";
      flake = false;
    };
    spdlog = {
      url = "https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz";
      flake = false;
    };
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils, ... }@inputs:
    flake-utils.lib.eachSystem [ "x86_64-linux" "x86_64-windows" ] (system:
      let
        pkgs = import nixpkgs {
          localSystem = { system = "x86_64-linux"; };
          config = { allowUnfree = true; };
        };
        pkgsCross = import nixpkgs {
          localSystem = { system = "x86_64-linux"; };
          crossSystem = {
            x86_64-linux = null;
            x86_64-windows = { config = "x86_64-w64-mingw32"; };
          }.${system};
          config = { allowUnfree = true; };
        };

        projectStdenv = {
          x86_64-linux = with pkgsCross;
            overrideCC stdenv llvmPackages_11.lldClang;
          x86_64-windows = with pkgsCross;
            let
              fun1 = old: {
                postFixup = old.postFixup + ''
                  > $out/nix-support/libc-cflags
                  for dir in ${old.cc}/lib/gcc/*/*/include{,-fixed}; do
                    echo "-isystem ''${dir}" >> $out/nix-support/libc-cflags
                  done
                  echo '-isystem ${old.libc_dev}/include' >> $out/nix-support/libc-cflags
                  substituteInPlace $out/nix-support/libc-cflags --replace $'\n' ' '
                '';
              };
              gcc_10 = gcc10Stdenv.cc.overrideAttrs fun1;
            in overrideCC gcc10Stdenv gcc_10;
        }.${system};

        projectBoost = (pkgsCross.boost174.override {
          stdenv = projectStdenv;
          zlib = pkgsCross.zlib.static;
          enableStatic = true;
          enableShared = false;
          taggedLayout = false;
        }).overrideAttrs (old:
          if projectStdenv.targetPlatform.isWindows then {
            configureFlags = old.configureFlags
              ++ [ "--with-toolset=cross-cxx" ];
            BUILD_CC = "${pkgs.stdenv.cc}/bin/gcc";
            BUILD_CXX = "${pkgs.stdenv.cc}/bin/g++";
            preBuild = ''
              sed -i '7,16d' project-config.jam
            '';
          } else {
            LD_LIBRARY_PATH =
              pkgs.lib.makeLibraryPath [ pkgs.llvmPackages_11.libunwind ];
          });
        wt = projectStdenv.mkDerivation {
          name = "wt";

          enableParalellBuilding = true;

          src = inputs.wt;

          nativeBuildInputs = with pkgs; [ cmake ];
          buildInputs = with pkgsCross;
            [ projectBoost zlib.static ]
            ++ lib.optionals projectStdenv.targetPlatform.isLinux [
              pango
              graphicsmagick
              libGLU
            ];
          cmakeFlags = [
            "--no-warn-unused-cli"
            "-DWT_CPP14_MODE=-std=c++14"
            "-DBOOST_DYNAMIC=OFF"
            "-DBUILD_TESTS=OFF"
            "-DBUILD_EXAMPLES=OFF"
            "-DSHARED_LIBS=OFF"
          ] ++ pkgs.lib.optionals projectStdenv.targetPlatform.isLinux [
            "-DWT_WRASTERIMAGE_IMPLEMENTATION=GraphicsMagick"
            "-DGM_PREFIX=${pkgs.graphicsmagick}"
          ];
        };
        dlib = projectStdenv.mkDerivation {
          name = "dlib";

          enableParallelBuilding = true;

          src = inputs.dlib;

          nativeBuildInputs = with pkgs; [ cmake ];
          buildInputs = with pkgsCross;
            lib.optionals projectStdenv.targetPlatform.isLinux [
              openblas
              lapack
              cudaPackages.cudatoolkit_11
              cudnn_cudatoolkit_11
            ];
          cmakeFlags = [
            "--no-warn-unused-cli"
            "-DUSE_AVX_INSTRUCTIONS=ON"
            "-DDLIBUSECUDA=ON"
            "-DDLIB_NO_GUI_SUPPORT=ON"
            "-DBUILD_SHARED_LIBS=OFF"
          ];
        };
        opencv = projectStdenv.mkDerivation {
          name = "opencv";

          enableParallelBuilding = true;

          src = inputs.opencv;

          nativeBuildInputs = with pkgs; [ cmake ];
          buildInputs = with pkgsCross;
            [ projectBoost zlib.static ]
            ++ lib.optionals projectStdenv.targetPlatform.isLinux [
              ffmpeg_4
              gst_all_1.gstreamer
              gst_all_1.gst-plugins-good
            ];
          cmakeFlags = [
            "--no-warn-unused-cli"
            "-DCPU_BASELINE=AVX2"
            "-DENABLE-CXX11=ON"
            "-DENABLE_PRECOMPILED_HEADERS=OFF"
            "-DBUILD_TESTS=OFF"
            "-DBUILD_EXAMPLES=OFF"
            "-DBUILD_PERF_TESTS=OFF"
            "-DBUILD_opencv_apps=OFF"
            "-DBUILD_SHARED_LIBS=OFF"
            "-DENABLE_PIC=OFF"
            "-DWITH_ZLIB=OFF"
            "-DWITH_GTK=OFF"
            "-DWITH_WIN32UI=OFF"
          ] ++ pkgsCross.lib.optionals projectStdenv.targetPlatform.isWindows
            [ "-DWITH_MSMF=OFF" ];
        };
        paho-mqtt-c = projectStdenv.mkDerivation {
          name = "paho-mqtt-c";

          src = inputs.paho-mqtt-c;

          preConfigure = ''
            substituteInPlace src/CMakeLists.txt \
              --replace RpcRT4 rpcrt4
          '';

          nativeBuildInputs = with pkgs; [ cmake ];
          cmakeFlags = [
            "--no-warn-unused-cli"
            "-DPAHO_BUILD_STATIC=ON"
            "-DPAHO_BUILD_SHARED=OFF"
          ];
          CFLAGS = pkgsCross.lib.optional projectStdenv.targetPlatform.isWindows
            "-D_WINDOWS";
        };
        paho-mqtt-cpp = projectStdenv.mkDerivation {
          name = "paho-mqtt-cpp";

          src = inputs.paho-mqtt-cpp;

          nativeBuildInputs = with pkgs; [ cmake ];
          buildInputs = [ paho-mqtt-c ];
          cmakeFlags = [
            "--no-warn-unused-cli"
            "-DPAHO_WITH_SSL=0"
            "-DPAHO_MQTT_C_LIBRARIES=${paho-mqtt-c}/lib/libpaho-mqtt3a${
              if projectStdenv.targetPlatform.isLinux then "" else "-static"
            }.a"
            "-DCMAKE_POLICY_DEFAULT_CMP0074=NEW"
            "-DPAHO_BUILD_STATIC=ON"
            "-DPAHO_BUILD_SHARED=OFF"
          ];
        };
        spdlog = projectStdenv.mkDerivation {
          name = "spdlog";

          src = inputs.spdlog;

          nativeBuildInputs = with pkgs; [ cmake ];
          cmakeFlags = [ "--no-warn-unused-cli" ];
        };
        domestic-supervisor = isShell:
          projectStdenv.mkDerivation rec {
            name = "domestic-supervisor";

            src = self;

            strictDeps = true;

            nativeBuildInputs = with pkgs;
              [ cmake ninja ] ++ lib.optional (!isShell) autoPatchelfHook;
            buildInputs =
              [ projectBoost wt dlib opencv paho-mqtt-cpp paho-mqtt-c spdlog ]
              ++ pkgsCross.lib.optionals projectStdenv.targetPlatform.isLinux [
                pkgsCross.openblas
                pkgsCross.graphicsmagick
                pkgsCross.libGL
              ];
            cmakeFlags = [
              "--no-warn-unused-cli"
              "-DPAHO_MQTT_C_LIBRARIES=${paho-mqtt-c}/lib/libpaho-mqtt3a${
                if projectStdenv.targetPlatform.isLinux then "" else "-static"
              }.a"
            ];

            MCFGTHREADS_DLL =
              pkgsCross.lib.optional projectStdenv.targetPlatform.isWindows
              "${pkgsCross.windows.mcfgthreads}/bin/mcfgthread-12.dll";
            PAHO_C_LIB = pkgsCross.lib.optional
              (isShell && projectStdenv.targetPlatform.isLinux)
              "${paho-mqtt-c}/lib/libpaho-mqtt3a.a";
            LD_LIBRARY_PATH = pkgsCross.lib.optional
              (isShell && projectStdenv.targetPlatform.isLinux)
              (pkgsCross.lib.makeLibraryPath (buildInputs ++ [
                pkgsCross.llvmPackages_11.libcxx
                pkgsCross.llvmPackages_11.libcxxabi
              ]));
            CPLUS_INCLUDE_PATH = pkgsCross.lib.optional
              (isShell && projectStdenv.targetPlatform.isLinux)
              (pkgs.lib.concatStringsSep ":" [
                "${pkgsCross.llvmPackages_11.libcxx}/include/c++/v1"
                "${pkgsCross.llvmPackages_11.libcxxabi}/include"
                "${projectStdenv.cc.cc}/lib/clang/${projectStdenv.cc.cc.version}/include"
                "${projectStdenv.cc.libc_dev}/include"
              ]);
            LDFLAGS =
              pkgsCross.lib.optional (projectStdenv.targetPlatform.isWindows)
              "-gstabs";
            dontStrip = true;
          };
        domestic-supervisor-non-nix = (domestic-supervisor false).overrideAttrs
          (old: {
            nativeBuildInputs = pkgsCross.lib.init old.nativeBuildInputs;
            postFixup = ''
              find $out -type f -exec patchelf --set-interpreter /lib64/ld-linux.so.2 {} \;
              find $out -type f -exec patchelf --set-rpath "/lib:/lib64:/usr/lib:/usr/lib64" {} \;
            '';
          });
      in {
        devShell = domestic-supervisor true;
        defaultPackage = domestic-supervisor true;
        packages = {
          domestic-supervisor = domestic-supervisor false;
          inherit opencv dlib wt paho-mqtt-c paho-mqtt-cpp spdlog projectBoost
            domestic-supervisor-non-nix;
        };
      });
}
