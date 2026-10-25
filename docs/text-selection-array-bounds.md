---
nav_exclude: true
---
# Text selection-array bounds

Text input widgets copy the configurable selection scan-type array into
widget-owned storage. The copy now checks the element-count multiplication
against both host `size_t` and Xt's `Cardinal` limits and handles allocation
failure before copying. If a count is invalid or storage cannot be allocated,
the widget falls back to Motif's built-in scan-type table, preserving
traversal behavior and the existing resource/API contract. Destruction leaves
that static fallback untouched instead of passing it to `XtFree`.

The warning-enabled TextIn source build verifies the guarded allocation path.
