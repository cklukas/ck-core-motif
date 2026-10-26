---
nav_exclude: true
---
# Container first-column bounds

Container outline/detail layouts derive the first column from an item width,
the outline-button width, and the item depth multiplied by the indentation.
Those values are ultimately represented by Motif's `Dimension` type.  The
calculation now saturates at the largest representable `Dimension` instead of
wrapping to a small width when a deeply nested tree or large display metric
exceeds that range.  Existing resource types, layout policy, and public ABI
are unchanged.

The source contract is checked by
`tests/automated/check-container-first-column-bounds.py`; the changed object is
also compiled in the native rpi4 incremental build.
