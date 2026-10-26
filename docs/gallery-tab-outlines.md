---
nav_exclude: true
---

# Gallery view-tab outlines

The component gallery's **Example**, **Source**, and **Built-in Icons** views
are all view tabs, not an unlabeled stack. Each tab receives a one-pixel
`#aeb7c2` outline. The outline remains visible when a tab is inactive, while
the selected tab keeps Motif's selected fill. This makes the available views
clear in the light appearance without adding another heavy beveled frame.

`tests/automated/check-gallery-tab-outlines.py` protects the three-tab styling
contract.
