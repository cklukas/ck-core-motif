---
layout: home
hero:
  name: CK Core Motif
  text: The maintained native Xt widget toolkit
  tagline: >
    CK Core Motif is a maintained fork of the Motif native Xt widget toolkit.
    It retains Motif's widget, resource and callback model while correcting
    build, selection, sizing and input defects and developing reusable native
    text and accessibility facilities.
  image:
    src: images/sun-workstation-hero.png
    alt: Pixel-art Sun workstation displaying a classic CDE and Motif desktop.
  actions:
    - text: Build the toolkit
      link: build.md
      theme: brand
    - text: Explore the APIs
      link: components.md
    - text: View CK Core Apps
      link: https://cklukas.github.io/ck-core-apps/
features:
  - icon: ":toolbox:"
    title: Native Xt widgets
    details: Maintained Motif widgets, resources and callbacks for native X11 applications.
    link: components.md
  - icon: ":capital_abcd:"
    title: Unicode text
    details: Explicit byte, scalar, grapheme and visual-position behavior throughout the text stack.
    link: unicode.md
  - icon: "♿"
    title: Native accessibility
    details: Semantic providers and optional AT-SPI integration without replacing the Xt model.
    link: accessibility.md
  - icon: ":white_check_mark:"
    title: Maintained and validated
    details: Focused regressions, native integration checks and documented qualification evidence.
    link: status.md
  - icon: ":desktop_computer:"
    title: CK Core Apps
    details: See the native desktop applications built on CK Core Motif and integrated with CK Core CDE.
    link: https://cklukas.github.io/ck-core-apps/
---

The development **2.4.0 / native ABI epoch 8** cohort is a deliberate binary
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
- [Maintenance overview](maintenance-overview.md) groups focused fixes and qualification notes by topic.
- [MWM overview](mwm-overview.md) groups window-management protocol and persistence notes.
- [Fork history](history.md) records the corrections preceding the new ABI cohort.

The toolkit follows the licensing terms in the repository's `COPYING` file.

See the [simple drag and drop demo](simpledrop.md) and
[DNDDemo drawing and color transfers](dnd-demo.md) for mouse gestures and validation.
