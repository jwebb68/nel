# This imports the nix package collection,
# so we can access the `pkgs` and `stdenv` variables
with import <nixpkgs> {};

# Make a new "derivation" that represents our shell
stdenv.mkDerivation {
  name = "nel";
  shellHook = ''
        export PS1="[nix-shell]$ "
  '';
  # packages that are for build only, and shouldnt be packaged for running
  nativeBuildInputs = [
    pkgs.gnumake42
    # astyle-3.1
    #pkgs.astyle
    # for clang-format
    pkgs.clang-tools
    pkgs.catch2
    pkgs.pkg-config
  ];
  # The packages in the `buildInputs` list will be added to the PATH in our shell
  buildInputs = [
  ];
  hardeningDisable = ["all"];
}
