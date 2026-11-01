---
nav_exclude: true
---
# Gadget default dimensions

When a Gadget receives no explicit width or height, Motif expands its base
size for highlight and shadow thickness. The calculation now uses a wide
intermediate and saturates at the representable `Dimension` maximum instead of
wrapping a large valid thickness into a small control. Ordinary dimensions are
unchanged.

The focused source contract is covered by
`tests/automated/check-gadget-default-dimensions.py`.
