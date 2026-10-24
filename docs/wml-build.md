# WML build reliability

The WML support tools are part of the Motif build and are required when the
UIL database is generated.  `wmluiltok` now has an explicit `main` entry point,
so it builds with current flex versions that no longer supply an implicit
scanner driver.

The generated `Uil.c`/`Uil.h` parser pair and the `UilLexPars.c`/
`UilLexPars.h` compatibility symlinks are created through stamp rules.  This
makes `make -j` deterministic: concurrent object prerequisites no longer run
two yacc recipes or race while creating the generated header.  The links point
to generated files in the same build directory, so out-of-tree builds do not
retain dangling source-tree links.  `make clean` removes the stamps and all
generated parser files.

The client UIL build links its generated parser source to the corresponding
`tools/wml` file in the build tree, not to a generated file that would have to
exist in the source checkout.

The WML specification lexer keeps its historical fixed-size semantic buffers,
but now checks identifier and quoted-string lengths before copying them. An
oversized token is reported as a lexer error instead of overwriting the next
parser buffer. The source contract is checked by
`tests/automated/check-wml-build.py`, and the normal native WML build exercises
the generated lexer.

`wmluiltok` also rejects more than ten synonyms for one token and more than
1000 token records before inserting into its fixed tables. The negative case is
covered by `tests/automated/run-wml-token-regression.py`; its error status keeps
malformed generator input from being treated as a successful build step.

The build now generates `wmluiltok.c` explicitly from the checked-in lexer and
generates the `Uil.c`/`Uil.h` parser pair from `Uil.y` before creating the
compatibility names used by `wmldbcreate`. This prevents stale generated files,
parallel `UilLexPars.h` collisions, and linker failures caused by a lexer object
that predates its `main` entry point.

The WML specification lexer (`wmllex.c`) is generated explicitly from
`wmllex.l` as part of the same out-of-tree build. A fresh checkout therefore
does not require a checked-in generated lexer; the selected `LEX` implementation
must be available when building the WML tools.

Generated lexer and parser outputs are not listed in `EXTRA_DIST`, so source
archives and fresh checkouts do not claim to contain files that are created
only by the build.
