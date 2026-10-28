---
nav_exclude: true
---

# Clipboard request format width

`ClipboardReplaceItem` now makes the validated format divisor explicitly
unsigned when computing incremental request sizes, avoiding an unintended
signed conversion in the X11 arithmetic.

Regression: `tests/automated/check-clipboard-request-format-width.py`
