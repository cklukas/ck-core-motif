---
nav_exclude: true
---
# TextField preedit repaint on failure

The TextField replacement and preedit replacement paths now repaint the
insertion point before returning from allocation-size or reallocation
failures.  A failed update no longer leaves the caret hidden after the routine
temporarily disables it.

Regression coverage: `tests/automated/check-textfield-preedit-repaint.py`.
