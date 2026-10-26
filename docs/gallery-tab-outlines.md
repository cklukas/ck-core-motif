---
nav_exclude: true
---

# Gallery view-tab outlines

The component gallery's **Example**, **Source**, and **Built-in Icons** views
are all view tabs, not an unlabeled stack. Each tab receives a one-pixel
`#aeb7c2` Motif shadow outline (with matching top and bottom colors). The
outline remains visible when a tab is inactive, while the selected tab keeps
Motif's selected fill. This makes the available views clear in the light
appearance without relying on an X window border that can disappear under a
theme or X server.

`tests/automated/check-gallery-tab-outlines.py` protects the three-tab styling
contract.
