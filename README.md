# CK Core Motif

A maintained fork of the Motif native Xt widget toolkit, with corrections for
modern builds, selection handling, display scaling and input behavior.

The development 2.4.0 cohort introduces native ABI epoch 5, shared Unicode text
captures and optional native accessibility. Rebuild every native consumer
against one consistent header/library cohort; this is not a drop-in replacement
for `libXm.so.4`.

Read the focused [documentation sources](docs/index.md),
[build guide](docs/build.md), [ABI policy](ABI.md) and
[validation status](docs/status.md). Historical manuals remain under `doc/`.
The documentation workflows target
[GitHub Pages](https://cklukas.github.io/ck-core-motif/) and the configured
self-hosted Pages service; workflow source alone does not establish publication.

See `COPYING` for licensing terms, `CHANGES` for earlier fork corrections and
`RELNOTES` for inherited release information.
