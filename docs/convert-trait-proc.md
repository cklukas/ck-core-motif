---
nav_exclude: true
---

# Conversion trait function pointers

The shared conversion handler obtains the widget’s optional transfer trait
before dispatching class-specific conversion.  It now requires both the trait
and its `convertProc` member to be present.  A partially populated trait is
treated as no conversion provider instead of being called through a null
function pointer.

The regression check is `tests/automated/check-convert-trait-proc.py`.
