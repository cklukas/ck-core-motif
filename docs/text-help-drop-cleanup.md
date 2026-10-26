---
nav_exclude: true
---
# Text help-drop cleanup

Text destination handling now releases the temporary drop location allocated
for `XmDROP_HELP` actions. The help path does not start a transfer, so retaining
that allocation leaked per-drop state until the destination callback ended.

Regression coverage: `tests/automated/check-text-help-drop-cleanup.py`.
