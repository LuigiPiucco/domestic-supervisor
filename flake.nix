{
  description = "Domestic Supervisor";
  inputs = {
    nixpkgs.url = "nixpkgs";
    wt = {
      url = "github:emweb/wt/4.5.0";
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
      url = "https://github.com/eclipse/paho.mqtt.cpp/archive/v1.2.0.tar.gz";
      flake = false;
    };
    paho-mqtt-c = {
      url = "https://github.com/eclipse/paho.mqtt.c/archive/v1.3.8.tar.gz";
      flake = false;
    };
    spdlog = {
      url = "https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz";
      flake = false;
    };
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils, ... }@inputs:
    flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let
        pkgs-linux = import nixpkgs {
          localSystem = { system = "x86_64-linux"; };
          config = { allowUnfree = true; };
        };
        pkgs-windows = import nixpkgs {
          localSystem = { system = "x86_64-linux"; };
          crossSystem = { config = "x86_64-w64-mingw32"; };
          config = {
            allowUnfree = true;
            packageOverrides = prev: {
              boost174 = (prev.boost174.override {
                stdenv = with prev; overrideCC stdenv buildPackages.gcc10;
              }).overrideAttrs (old: {
                preConfigure = old.preConfigure + "export CXX=${prev.pkgsBuildBuild.gcc10}/bin/g++";
                preBuild = "export CXX=${prev.buildPackages.gcc10}/bin/g++";
              });
            };
          };
        };

        wt-fn = pkgs:
          let stdenv' = with pkgs; overrideCC stdenv buildPackages.gcc10;
          in stdenv'.mkDerivation {
            pname = "wt";
            version = "4.5.0";

            enableParalellBuilding = true;

            src = inputs.wt;

            nativeBuildInputs = with pkgs.buildPackages; [ cmake ];
            buildInputs = (with pkgs;
              [ zlib boost174 ] ++ lib.optionals targetPlatform.isLinux [
                pango
                graphicsmagick
                libGLU
              ]);
            cmakeFlags = [
              "--no-warn-unused-cli"
              "-DWT_CPP14_MODE=-std=c++14"
              "-DBUILD_TESTS=OFF"
              "-DBUILD_EXAMPLES=OFF"
            ] ++ pkgs.lib.optionals pkgs.targetPlatform.isLinux [
              "-DWT_WRASTERIMAGE_IMPLEMENTATION=GraphicsMagick"
              "-DGM_PREFIX=${pkgs.graphicsmagick}"
            ];
          };

        paho-mqtt-c-fn = pkgs:
          let stdenv' = with pkgs; overrideCC stdenv buildPackages.gcc10;
          in stdenv'.mkDerivation {
            pname = "paho-mqtt-c";
            version = "1.3.8";

            src = inputs.paho-mqtt-c;

            preConfigure = ''
              substituteInPlace src/CMakeLists.txt \
                --replace RpcRT4 rpcrt4
            '';

            nativeBuildInputs = with pkgs.buildPackages; [ cmake ];
            cmakeFlags = [
              "--no-warn-unused-cli"
              "-DPAHO_BUILD_STATIC=ON"
            ];
            CFLAGS = with pkgs; lib.optional targetPlatform.isWindows
              "-D_WINDOWS";
          };

        paho-mqtt-cpp-fn = pkgs:
          let stdenv' = with pkgs; overrideCC stdenv buildPackages.gcc10;
              paho-mqtt-c = paho-mqtt-c-fn pkgs;
          in stdenv'.mkDerivation {
            pname = "paho-mqtt-cpp";
            version = "1.2.0";

            src = inputs.paho-mqtt-cpp;

            nativeBuildInputs = with pkgs.buildPackages; [ cmake ];
            buildInputs = [ paho-mqtt-c ];
            cmakeFlags = [
              "--no-warn-unused-cli"
              "-DPAHO_WITH_SSL=0"
              "-DPAHO_MQTT_C_LIBRARIES=${paho-mqtt-c}/lib/libpaho-mqtt3a${with pkgs; lib.optionalString targetPlatform.isWindows ".dll"}.a"
              "-DPAHO_BUILD_STATIC=ON"
              "-DCMAKE_POLICY_DEFAULT_CMP0074=NEW"
            ];
          };

        dlib-fn = pkgs:
          let stdenv' = with pkgs; overrideCC stdenv buildPackages.gcc10;
          in stdenv'.mkDerivation {
            pname = "dlib";
            version = "19.21";

            enableParalellBuilding = true;

            src = inputs.dlib;

            nativeBuildInputs = with pkgs.buildPackages; [ cmake ];
            buildInputs = with pkgs;
              lib.optionals targetPlatform.isLinux [
                openblas
                lapack
                cudaPackages.cudatoolkit_11_0
                cudnn_cudatoolkit_11_0
              ];
            cmakeFlags = [
              "--no-warn-unused-cli"
              "-DUSE_AVX_INSTRUCTIONS=ON"
              "-DDLIBUSECUDA=ON"
              "-DDLIB_NO_GUI_SUPPORT=ON"
              "-DBUILD_SHARED_LIBS=OFF"
              "-DCMAKE_CUDA_COMPILER_ID=NVIDIA"
            ];
          };

        opencv-fn = pkgs:
          let stdenv' = with pkgs; overrideCC stdenv buildPackages.gcc10;
          in stdenv'.mkDerivation {
            pname = "opencv";
            version = "4.5.0";

            enableParalellBuilding = true;

            src = inputs.opencv;

            nativeBuildInputs = with pkgs.buildPackages; [ cmake ];
            buildInputs = with pkgs;
              [ boost174 zlib ]
              ++ lib.optionals targetPlatform.isLinux [
                ffmpeg_4
                gst_all_1.gstreamer
                gst_all_1.gst-plugins-good
              ];
              cmakeFlags = with pkgs; [
                "--no-warn-unused-cli"
                "-DENABLE-CXX11=ON"
                "-DENABLE_PRECOMPILED_HEADERS=OFF"
                "-DBUILD_TESTS=OFF"
                "-DBUILD_EXAMPLES=OFF"
                "-DBUILD_PERF_TESTS=OFF"
                "-DBUILD_opencv_apps=OFF"
                "-DBUILD_SHARED_LIBS=OFF"
                "-DWITH_GTK=OFF"
                "-DWITH_WIN32UI=OFF"
              ] ++ lib.optional targetPlatform.isWindows [
                "-DBUILD_ZLIB=OFF"
                "-DWITH_MSMF=OFF"
                "-DENABLE_PIC=OFF"
              ] ++ lib.optional targetPlatform.isLinux "-DCPU_BASELINE=AVX2";
            };

        spdlog-fn = pkgs:
          let stdenv' = with pkgs; overrideCC stdenv buildPackages.gcc10;
          in stdenv'.mkDerivation {
            pname = "spdlog";
            version = "1.8.1";

            enableParalellBuilding = true;

            src = inputs.spdlog;

            nativeBuildInputs = with pkgs.buildPackages; [ cmake ];
          };

        domestic-supervisor-fn = pkgs:
          let stdenv' = with pkgs; overrideCC stdenv buildPackages.gcc10;
              paho-mqtt-c = paho-mqtt-c-fn pkgs;
              paho-mqtt-cpp = paho-mqtt-cpp-fn pkgs;
              wt450 = wt-fn pkgs;
              dlib1921 = dlib-fn pkgs;
              opencv450 = opencv-fn pkgs;
              spdlog181 = spdlog-fn pkgs;
          in stdenv'.mkDerivation rec {
            name = "domestic-supervisor";

            src = self;

            strictDeps = true;

            nativeBuildInputs = with pkgs.buildPackages; [ cmake ninja ];
            buildInputs = with pkgs;
              [ boost174 paho-mqtt-cpp paho-mqtt-c wt450 dlib1921 opencv450 spdlog181 ]
              ++ lib.optionals targetPlatform.isLinux [
                openblas
                graphicsmagick
                libGL
              ];
            cmakeFlags = [
              "--no-warn-unused-cli"
              "-DPAHO_MQTT_C_LIBRARIES=${paho-mqtt-c}/lib/libpaho-mqtt3a${with pkgs; lib.optionalString targetPlatform.isWindows ".dll"}.a"
            ];

            NIX_CFLAGS_LINK = with pkgs; lib.optionalString targetPlatform.isWindows "-static-libstdc++ -static-libgcc";
            EXTRA_DLLS = with pkgs;
              lib.optionalString targetPlatform.isWindows
                "${windows.mcfgthreads}/bin/mcfgthread-12.dll;${zlib}/bin/zlib1.dll;${paho-mqtt-c}/bin/libpaho-mqtt3a.dll";
            # for devShell
            PAHO_C_LIB =
                "${paho-mqtt-c}/lib/libpaho-mqtt3a${with pkgs; lib.optionalString targetPlatform.isWindows ".dll"}.a";
          };

        domestic-supervisor-linux = domestic-supervisor-fn pkgs-linux;
        domestic-supervisor-windows = domestic-supervisor-fn pkgs-windows;

      in {
        devShell = domestic-supervisor-linux;
        defaultPackage = domestic-supervisor-linux;
        packages = {
          inherit domestic-supervisor-linux domestic-supervisor-windows;
        };
      });
}
