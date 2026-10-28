---
nav_exclude: true
---

# DragUnder pixmap dimensions

The internal saved-pixmap record now stores width and height as Motif
`Dimension`, matching the non-wide `_XmAllocScratchPixmap` interface and the
source `SaveAll`/`SaveSegments` parameters. This removes avoidable narrowing
while preserving the existing internal layout and rendering behavior.

Regression: `tests/automated/check-dragunder-pixmap-dimensions.py`
