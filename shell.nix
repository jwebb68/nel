# This imports the nix package collection,
# so we can access the `pkgs` and `stdenv` variables
with import <nixpkgs> {};

# Make a new "derivation" that represents our shell
pkgs.mkShell {
  name = "nel";
  shellHook = ''
        export PS1="[nix-shell]$ "
  '';
  # packages that are for build only, and shouldnt be packaged for running
  nativeBuildInputs = [
    pkgs.gnumake42
    # astyle-3.1
    pkgs.astyle
    # for clang-format
    pkgs.clang-tools
    pkgs.catch2
    pkgs.check
    pkgs.pkg-config

    # multi is not multi version of compiler, but multilib
    #pkgs.clang_multi
    pkgs.clang

    #pkgs.gcc-arm-embedded-7
    #pkgs.gcc-arm-embedded-8
    #pkgs.gcc-arm-embedded-9
    pkgs.gcc-arm-embedded

    # pkgs.gcc7
    # pkgs.gcc8
    pkgs.gcc
    # multi is not multi version of compiler, but multilib
    # pkgs.gcc_multi

  ];
  # The packages in the `buildInputs` list will be added to the PATH in our shell
  buildInputs = [
  ];
  hardeningDisable = ["all"];
}
