---
nav_exclude: true
---
# Drop-transfer size-limit header

The drop-transfer table growth guard uses `SIZE_MAX` to reject unrepresentable
allocation sizes. `DropTrans.c` now includes `<stdint.h>`, the standard header
that defines the macro, so this guard compiles portably alongside the existing
allocation checks.

The focused regression is
`tests/automated/check-drop-transfer-size-limit-header.py`.
