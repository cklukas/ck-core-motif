---
nav_exclude: true
---
# Text selection-array bounds

Text input widgets copy the configurable selection scan-type array into
widget-owned storage.  The copy now checks the element-count multiplication
before allocating.  If an invalid count cannot be represented safely, the
widget falls back to Motif's built-in scan-type table, preserving traversal
behavior and the existing resource/API contract.

The warning-enabled TextIn source build verifies the guarded allocation path.
