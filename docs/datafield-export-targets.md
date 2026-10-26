---
nav_exclude: true
---
# DataField export targets

DataField drop handling now rejects a drag context that reports export targets
but supplies no target array. The drop transfer is marked failed before any
target iteration occurs.

Regression coverage: `tests/automated/check-datafield-export-targets.py`.
