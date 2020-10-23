let wtVersion = "4.4.0";
in {
  description = "Domestic Supervisor";
  inputs = {
    nixpkgs.url = "nixpkgs";
    wt = {
      url = "github:emweb/wt/${wtVersion}";
      flake = false;
    };

  };
  outputs = { self, nixpkgs, ... }@inputs:
    let
      pkgs = import nixpkgs { system = "x86_64-linux"; };

      wt = pkgs.stdenv.mkDerivation {
        name = "wt";
        version = wtVersion;

        enableParalellBuilding = true;

        src = inputs.wt;

        buildInputs = with pkgs; [ cmake boost pango graphicsmagick ];
        cmakeFlags = [
          "--no-warn-unused-cli"
          "-DWT_CPP14_MODE=-std=c++14"
          "-DWT_WRASTERIMAGE_IMPLEMENTATION=GraphicsMagick"
        ];
      };
      domestic-supervisor = pkgs.stdenv.mkDerivation {
        name = "domestic-supervisor";

        src = self;

        buildInputs = with pkgs; [ cmake wt ];
        cmakeFlags = [ "--no-warn-unused-cli" ];

        CPLUS_INCLUDE_PATH =
          "${pkgs.llvmPackages_11.libcxx}/include/c++/v1:${pkgs.clang_11.libc_dev}/include";
      };
    in { defaultPackage.x86_64-linux = domestic-supervisor; };
}
