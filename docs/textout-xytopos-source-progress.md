---
nav_exclude: true
---
# TextOut coordinate hit-testing source validation

`XYToPos` now rejects non-advancing or malformed text-source blocks before
character measurement. This keeps coordinate hit testing bounded when a
source implementation fails to provide a valid block.

Regression coverage: `tests/automated/check-textout-xytopos-source-progress.py`.
