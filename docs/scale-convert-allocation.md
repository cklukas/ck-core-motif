---
nav_exclude: true
---
# XmScale selection conversion allocation

XmScale conversion now refuses a target when its target list, plain-string,
or compound-text result cannot be allocated. It also handles a failed X11
compound-text conversion before copying the result, preserving successful
clipboard and selection conversion behavior.

`tests/automated/check-scale-convert-allocation.py` protects this contract.
