{
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/ae814fd3904b621d8ab97418f1d0f2eb0d3716f4";
  };

  outputs = { self, nixpkgs }:
  let
    system = "x86_64-linux";

    pkgs = import nixpkgs {
      inherit system;
    };
  in {
    devShells.${system}.default = pkgs.mkShellNoCC rec {
      nativeBuildInputs = with pkgs; [
        gcc
      ];

      buildInputs = with pkgs; [
        libGL
        libx11
        libxcursor
        libxext
        libxrandr
        libxi
        libxinerama
      ];

      LD_LIBRARY_PATH = pkgs.lib.makeLibraryPath buildInputs;
    };
  };
}
