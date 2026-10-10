# -*- mode: nix; indent-tabs-mode: nil; tab-width: 2 -*-
{
  description = "A devShell for nel";

  inputs = {
    nixpkgs.url      = "github:NixOS/nixpkgs/nixos-23.11";
    flake-utils.url  = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = import nixpkgs {
          inherit system;
        };
        build_inputs = [];
        native_build_inputs = [
            pkgs.gcc
            pkgs.gdb
            pkgs.gcc-arm-embedded-10 #ok

            pkgs.clang_15
            # for clang-format,
            # keep in sync with pkgs.clang
            pkgs.clang-tools_15

            pkgs.catch2
            pkgs.check
            pkgs.pkg-config

            # astyle-3.1
            pkgs.astyle
        ];
      in
      {
        devShells.default = pkgs.mkShell {
          buildInputs = build_inputs;
          nativeBuildInputs = native_build_inputs;

          shellHook = ''
            export PS1="[nix-shell]$ "
          '';

          hardeningDisable = [ "all" ];
        };
      }
    );
}
