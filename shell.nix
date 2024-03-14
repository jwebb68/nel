# -*- mode: nix; indent-tabs-mode: nil; tab-width: 2 -*-
(builtins.getFlake ("git+file://" + toString ./.)).devShells.${builtins.currentSystem}.default

