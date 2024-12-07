# This imports the nix package collection,
# so we can access the `pkgs` and `stdenv` variables
{ pkgs ? import <nixpkgs> { } }:

let
  # Pinned nixpkgs, deterministic. Last updated: 2/12/21.
  pkgs_pinned = import (fetchTarball
    #("https://github.com/NixOS/nixpkgs/archive/a58a0b5098f0c2a389ee70eb69422a052982d990.tar.gz"))
    #("https://github.com/NixOS/nixpkgs/archive/a58a0b5098f0c2a389ee70eb69422a052982d990.tar.gz"))
    ("https://github.com/NixOS/nixpkgs/archive/refs/tags/23.05.tar.gz"))
    { };

  # Rolling updates, not deterministic.
  # pkgs = import (fetchTarball("channel:nixpkgs-unstable")) {};
in pkgs_pinned.mkShell {
  shellHook = ''
    export PS1="[nix-shell]$ "
  '';

  # packages that are for build only, and shouldnt be packaged for running
  nativeBuildInputs = [

    pkgs_pinned.gnumake42

    # pkgs_pinned.gcc7
    # pkgs_pinned.gcc8
    pkgs_pinned.gcc

    # multi is not multi version of compiler, but multilib
    # pkgs_pinned.gcc_multi

    pkgs_pinned.gdb

    # pkgs_pinned.gcc-arm-embedded-7 #ok
    # pkgs_pinned.gcc-arm-embedded-8 #ok
    # pkgs_pinned.gcc-arm-embedded-9 #ok
    pkgs_pinned.gcc-arm-embedded-10 # ok
    # pkgs_pinned.gcc-arm-embedded-11 #fails: libcrypt.so.1 missing
    # pkgs_pinned.gcc-arm-embedded #fails: libcrypt.so.1 missing

    # multi is not multi version of compiler, but multilib
    #pkgs_pinned.clang_multi
    pkgs_pinned.clang_15
    # for clang-format,
    # keep in sync with pkgs_pinned.clang
    pkgs_pinned.clang-tools_15

    pkgs_pinned.pkgconfig

    pkgs_pinned.catch2
    pkgs_pinned.check
    pkgs_pinned.pkg-config

    # astyle-3.1
    pkgs_pinned.astyle
  ];

  # The packages in the `buildInputs` list will be added to the PATH in our shell
  buildInputs = [ ];

  hardeningDisable = [ "all" ];

}
