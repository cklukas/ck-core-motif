# CK Core Motif

A maintained fork of the Motif native Xt widget toolkit, with corrections for
modern builds, selection handling, display scaling and input behavior.

The development 2.4.0 cohort introduces native ABI epoch 7, shared Unicode text
captures and optional native accessibility. Rebuild every native consumer
against one consistent header/library cohort; this is not a drop-in replacement
for `libXm.so.4`.

The fresh installed ABI7 cohort passes all 19 native ARM64 feature fixtures,
including native list identities, Selection, real keyboard range selection and
allocation-failure recovery. Repeated real IBus/m17n service validation has an
intermittent Hindi preedit failure under investigation. Sanitizer qualification and actual application
adoption of this cohort remain separate gates; x86_64 validation is pending.

Read the focused [documentation sources](docs/index.md),
[build guide](docs/build.md), [ABI policy](ABI.md) and
[validation status](docs/status.md). Historical manuals remain under `doc/`.
The documentation workflows target
[GitHub Pages](https://cklukas.github.io/ck-core-motif/) and the configured
self-hosted Pages service; workflow source alone does not establish publication.

See `COPYING` for licensing terms, `CHANGES` for earlier fork corrections and
`RELNOTES` for inherited release information.
