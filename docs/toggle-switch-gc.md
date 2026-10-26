---
nav_exclude: true
---
# ToggleSwitch graphics-context setup

`XmToggleSwitch` now cleans up any graphics contexts already created when one
of its four GC allocations fails. All GC fields are reset to `NULL`, so expose
and destroy paths remain safe even when the server cannot provide the complete
set of drawing resources.

The focused source contract is checked by
`tests/automated/check-toggle-switch-gc.py`.
