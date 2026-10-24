# WML build reliability

The WML support tools are part of the Motif build and are required when the
UIL database is generated.  `wmluiltok` now has an explicit `main` entry point,
so it builds with current flex versions that no longer supply an implicit
scanner driver.

The generated `UilLexPars.c` and `UilLexPars.h` symlinks are created through a
single stamp rule.  This makes `make -j` deterministic: concurrent object
prerequisites no longer run two recipes that race to create the same header.
The stamp recipe removes stale links before recreating them, so an interrupted
build can be resumed safely.  The client-side UIL parser link also points back
to the source tree, which keeps out-of-tree builds from retaining a dangling
relative symlink.  `make clean` removes the stamp as well as the generated
links.

The WML specification lexer keeps its historical fixed-size semantic buffers,
but now checks identifier and quoted-string lengths before copying them. An
oversized token is reported as a lexer error instead of overwriting the next
parser buffer. The source contract is checked by
`tests/automated/check-wml-build.py`, and the normal native WML build exercises
the generated lexer.
