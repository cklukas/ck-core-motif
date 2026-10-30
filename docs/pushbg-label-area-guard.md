---
nav_exclude: true
---

# PushButtonGadget label-area guard

`ComputePBLabelArea` now validates both the gadget and output rectangle before
reading gadget colors or geometry. Invalid internal calls return `False`
without dereferencing state; valid layout calculations retain their existing
geometry and non-negative sizing behavior.

The focused source-contract check is
`python3 tests/automated/check-pushbg-label-area-guard.py`.
