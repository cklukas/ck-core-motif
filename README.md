# CK Core Motif

Development source now requires native ABI8: TextField owns an incremental Unicode
map cache. Rebuild toolkit libraries and all consumers together; see the
[ABI policy](https://cklukas.github.io/ck-core-motif/abi.html).
Installed epoch8 qualification remains separate from the historical results below.

A maintained fork of the Motif native Xt widget toolkit, with corrections for
modern builds, selection handling, display scaling and input behavior.

The development 2.4.0 cohort uses native ABI epoch 8, including the current
Unicode text maps and optional native accessibility records. Rebuild every
native consumer against one consistent header/library cohort; this is not a
drop-in replacement for `libXm.so.4`.

The fresh installed ABI8 cohort passes the selected native feature fixtures,
including native list identities, Selection, Unicode text maps, accessibility,
display-scale and allocation-failure recovery. External input-method,
application adoption and full cross-architecture validation remain separate
gates; system/Xt ownership remains outside the toolkit sanitizer scope.

Read the [documentation](https://cklukas.github.io/ck-core-motif/),
[build guide](https://cklukas.github.io/ck-core-motif/build.html),
[ABI policy](https://cklukas.github.io/ck-core-motif/abi.html) and
[validation status](https://cklukas.github.io/ck-core-motif/status.html).
Historical manuals remain under `doc/`. The ckdocs workflows publish the site
to GitHub Pages and the configured self-hosted Pages service.

See `COPYING` for licensing terms, `CHANGES` for earlier fork corrections and
`RELNOTES` for inherited release information.
