# CK Core Motif

CK Core Motif is a maintained fork of the Motif native Xt widget toolkit.
It retains Motif's widget, resource and callback model while correcting
build, selection, sizing and input defects and developing reusable native
text and accessibility facilities.

The development **2.4.0 / native ABI epoch 7** cohort is a deliberate binary
break. Its headers and libraries must be installed together, and every native
consumer must be rebuilt. It is not a drop-in replacement for `libXm.so.4`.
Read the [ABI policy](abi.md) before selecting a development prefix.

These pages describe this fork's changes and APIs. The historical manuals and
demos remain in the repository's `doc/` and `demos/` directories; this site does
not regenerate or import that entire reference collection.

- [Build and install](build.md) into an isolated prefix.
- [Unicode text](unicode.md) distinguishes bytes, scalars, graphemes and visual positions.
- [Accessibility](accessibility.md) describes native semantic providers and optional AT-SPI.
- [Validation status](status.md) distinguishes implemented source from verified behavior.
- [Fork history](history.md) records the corrections preceding the new ABI cohort.

The toolkit follows the licensing terms in the repository's `COPYING` file.
