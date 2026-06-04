{
  description = "sakukernel development environment";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        packages = with pkgs; [
          qemu
          gnumake
          gcc
          nasm
          binutils
          gdb
        ];

        shellHook = ''
            export SAKU_NIX_SHELL=sakukernel
        '';
      };
    };
}
