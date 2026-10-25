---
nav_exclude: true
---
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

On 2026-10-25, an isolated AArch64 `uconsole` bootstrap copied the source to a
fresh temporary tree, ran `autoreconf -fi`, configured with the optional text,
Xft, AT-SPI and XLibre features disabled, generated the normal `makestrs` and
XmString prerequisites, built `libXm.la`, and then ran `make -j2 -C
tools/wml all`. The build generated `wmllex.c` from `wmllex.l`, generated the
UIL parser and compatibility links, built `wml`, `wmluiltok` and `wmldbcreate`,
and created `motif.wmd`. The recorded hashes were:

| Input/output | SHA-256 |
| --- | --- |
| `tools/wml/wmllex.l` | `1b09024869b107af3b4c4e5d3fe8500d1ab9e26b0eec7b5116d6a67fb0762fb3` |
| generated `tools/wml/wmllex.c` | `5b6335ab2f31e9ec3110fee255c08c54e7ad114eb6ffed72c536925ac8b38476` |

This confirms that the absent source-tree `wmllex.c` is intentional and that
the out-of-tree rule supplies it. The bootstrap still emits pre-existing
`-Wall` format/legacy-cast warnings in unrelated Motif/WML sources; no
missing-lexer or generated-parser error occurred, and the warnings are not
described as a warning-free build.

The generated C file can contain build-directory context from the selected
`flex` implementation, so its digest is evidence for one build result rather
than a source reproducibility key. A later incremental `uconsole` build using
`flex 2.6.4` (GCC 14.2.0) retained the same lexer-source digest and generated
the following output:

| Input/output | SHA-256 |
| --- | --- |
| `tools/wml/wmllex.l` | `1b09024869b107af3b4c4e5d3fe8500d1ab9e26b0eec7b5116d6a67fb0762fb3` |
| generated `tools/wml/wmllex.c` | `8774317dc630ca4052891b75149f40fdf6300d043135e5ceccabf5190ed35810` |

The stable acceptance condition is successful generation and compilation from
the checked-in `.l` source; generated-output hashes remain useful provenance
for the exact build directory and toolchain recorded alongside them.

The WML key-table emitter now converts its bounded keyword length once to the
`int` field required by the generated table, eliminating the `size_t`/`%d`
format mismatch while retaining the existing generated output. The source
contract verifier checks this conversion.

UIL modules that export only literals or icons have no top-level widget
descriptors. `UrmIFMInitModule` now writes the valid empty descriptor table for
that case, so the `demos/programs/animate/dog.uil` full-tree target succeeds;
`tests/automated/run-uil-zero-topmost.py` is the focused regression check.

Generated lexer and parser outputs are not listed in `EXTRA_DIST`, so source
archives and fresh checkouts do not claim to contain files that are created
only by the build.
