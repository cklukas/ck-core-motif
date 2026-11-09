---
nav_exclude: true
---
# WML build reliability

The WML support tools are part of the Motif build and are required when the
UIL database is generated.  `wmluiltok` now has an explicit `main` entry point,
so it builds with current flex versions that no longer supply an implicit
scanner driver.

The Exm WML database generator likewise uses typed C definitions for its
entry point and emitters, rather than K&R declarations.  This preserves its
database output while allowing the `wmldbcreate` object to compile under the
current C23 configuration.  `tests/automated/check-wmldbcreate-c23.py` checks
the relevant source contracts.

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

The keyword-table emitter also handles WML names at the lexer-supported input
length. It now allocates the temporary case-folded name and generated
`sym_k_...` literal from the measured input, rather than placing either in a
100-byte stack buffer. This preserves generated output for ordinary names and
stops generation on allocation failure without overwriting generator state. The focused
source regression is `tests/automated/check-wml-keyword-buffer-bounds.py`.

The `tokens.dat` reader likewise limits each generated grammar-token field to
the capacity of its legacy record buffer and uses a temporary allocated copy
when lowercasing keyword spellings. It fails cleanly if either token allocation
fails; `tests/automated/check-wml-grammar-token-bounds.py` covers that contract.

WML's shared dynamic string-copy helper now returns `NULL` when allocation
fails, before attempting a copy. This prevents a low-memory generator run from
turning an ordinary allocation failure into a null-pointer write; the focused
regression is `tests/automated/check-wml-string-allocation.py`.

Resolver diagnostics now use bounded formatting for class, character-set, and
reference names, so malformed long names cannot overwrite the diagnostic stack
buffer. The regression is `tests/automated/check-wml-diagnostic-bounds.py`.

The WML class-mask emitters write each `_BIT_MASK` term directly to the
generated header instead of assembling a complete row in fixed 100- and
500-byte buffers. Long valid class names can therefore be emitted without
overwriting generator storage, while the established generated header layout is
unchanged. `tests/automated/check-wml-class-mask-bounds.py` checks this source
contract; the native WML target verifies the generated headers.

Character-set `$LANG` names and aliases are likewise copied into an
input-sized uppercase buffer before emission. A failed allocation stops WML
generation instead of writing through the former 200-byte local array or
continuing with a partial header. The source regression is
`tests/automated/check-wml-charset-name-bounds.py`.

The `wmluiltok` lexer now copies token fields through a checked helper. A
low-memory token parse terminates with a diagnostic rather than applying
`strcpy` to a null allocation result. The focused source regression is
`tests/automated/check-wml-lexer-copy-failure.py`; the native WML target
regenerates and runs this lexer.

WML's central dynamic handle-list allocation now checks its requested element
count and allocation result before publishing list state. Initial allocation
and growth fail with a diagnostic rather than leaving a null or undersized
vector to be dereferenced later. The focused regression is
`tests/automated/check-wml-handle-list-allocation.py`.

`wmldbcreate` validates its dynamic string-length table count and allocation,
and rejects strings that cannot be represented by the established `int` length
field. This prevents a failed allocation or narrowing overflow from corrupting
database creation. The focused regression is
`tests/automated/check-wmldb-length-allocation.py`.

WML syntax construction now routes descriptor and alias-vector allocation
through checked helpers. Invalid sizes and allocation failures stop parsing
before any incomplete object is linked into parser state. The focused
regression is `tests/automated/check-wml-syntax-builder-allocation.py`.

WML handle-list insertion now requires a successful copy of its lookup name
before publishing the entry, avoiding null-name comparisons after low-memory
failures. The focused regression is
`tests/automated/check-wml-handle-list-name-copy.py`.

WML semantic-resolution objects are allocated through a checked helper, so a
low-memory failure stops resolution before any partially initialized object is
linked into the generated model. The focused regression is
`tests/automated/check-wml-resolver-allocation.py`.

WML resolver symbol codes now use full-width integers internally. This avoids
silent signed-16-bit truncation if a generated catalogue exceeds 32,767 entries;
existing catalogue output remains unchanged. The focused regression is
`tests/automated/check-wml-symbol-code-width.py`.

The WML grammar-token reader rejects negative, oversized, and duplicate IDs
before it inserts into its fixed token vector, and frees rejected records. The
focused regression is `tests/automated/check-wml-token-id-bounds.py`.

The WML keyword-table and manual writers emit fixed text through `fputs` or
literal `fprintf` formats, rather than treating a mutable template as a runtime
format string. The focused regression is
`tests/automated/check-wml-output-format-safety.py`.

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

WML's internal `ObjectPtr` is a standard `void *` generic object pointer. This
keeps the heterogeneous handle and token tables type-correct when current C
compilers diagnose unrelated structure-pointer arguments instead of accepting
the historical pre-ANSI `char *` convention. The representation and generated
tables are unchanged, and the source contract verifier rejects a regression to
the character-pointer definition.

UIL modules that export only literals or icons have no top-level widget
descriptors. `UrmIFMInitModule` now writes the valid empty descriptor table for
that case, so the `demos/programs/animate/dog.uil` full-tree target succeeds;
`tests/automated/run-uil-zero-topmost.py` is the focused regression check.

Generated lexer and parser outputs are not listed in `EXTRA_DIST`, so source
archives and fresh checkouts do not claim to contain files that are created
only by the build.

`tests/automated/check-wmd.py` performs a source-only WMD contract check when
run without arguments, which keeps the broad contract sweep independent of a
generated build tree. Pass an absolute build root and fresh results directory
to run the executable generation and UIL round-trip checks.
