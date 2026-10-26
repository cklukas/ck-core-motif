---
nav_exclude: true
---

# Text conversion callback inputs

The Text and TextField conversion callback wrappers now reject a null widget
or callback structure before encoding lookup, display access, or selection
field access. This keeps malformed owner requests and teardown callbacks from
crashing before the lower-level converter guards can run.

The regression check is
`tests/automated/check-text-convert-callback-inputs.py`.
