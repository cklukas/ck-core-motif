---
nav_exclude: true
---
# TextOut measurement block validation

The width and height measurement entry points now reject null, empty, or
invalid text blocks before dispatching to font, Xft, or multibyte routines.
This makes malformed source data harmless at the common measurement boundary.

Regression coverage: `tests/automated/check-textout-measurement-blocks.py`.
