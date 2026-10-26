---
nav_exclude: true
---
# XmText PreeditDraw abort cleanup

When preedit verification rejects an update, or the text source rejects the
replacement, `XmText` now releases the temporary converted string and runs
the normal reset cleanup before returning.  The source-replace failure path
also restores insertion-point painting.

Regression coverage: `tests/automated/check-text-preedit-draw-cleanup.py`.
