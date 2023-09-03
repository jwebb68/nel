# This imports the nix package collection,
# so we can access the `pkgs` and `stdenv` variables
{ pkgs ? import <nixpkgs> {} }:

  let
    # Pinned nixpkgs, deterministic. Last updated: 2/12/21.
    #pkgs = import (fetchTarball("https://github.com/NixOS/nixpkgs/archive/a58a0b5098f0c2a389ee70eb69422a052982d990.tar.gz")) {};

    # Rolling updates, not deterministic.
    # pkgs = import (fetchTarball("channel:nixpkgs-unstable")) {};
  in pkgs.mkShell {
    shellHook = ''
          export PS1="[nix-shell]$ "
    '';

  # packages that are for build only, and shouldnt be packaged for running
    nativeBuildInputs = [

      pkgs.gnumake42

      # pkgs.gcc7
      # pkgs.gcc8
      pkgs.gcc

      # multi is not multi version of compiler, but multilib
      # pkgs.gcc_multi

      pkgs.gdb

      # pkgs.gcc-arm-embedded-7 #ok
      # pkgs.gcc-arm-embedded-8 #ok
      # pkgs.gcc-arm-embedded-9 #ok
      pkgs.gcc-arm-embedded-10 #ok
      # pkgs.gcc-arm-embedded-11 #fails: libcrypt.so.1 missing
      # pkgs.gcc-arm-embedded #fails: libcrypt.so.1 missing

      # multi is not multi version of compiler, but multilib
      #pkgs.clang_multi
      pkgs.clang_15
      # for clang-format,
      # keep in sync with pkgs.clang
      pkgs.clang-tools_15


      pkgs.pkgconfig

      pkgs.catch2
      pkgs.check
      pkgs.pkg-config

      # astyle-3.1
      pkgs.astyle
    ];

    # The packages in the `buildInputs` list will be added to the PATH in our shell
    buildInputs = [
    ];

    hardeningDisable = ["all"];

  }
