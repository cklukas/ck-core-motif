---
nav_exclude: true
---

# XIM preedit feedback length

`PreeditSetRendition` iterates the feedback array using
`XIMText.length`, whose type is `int`.  Its former `unsigned short` counter
could wrap for long feedback arrays and repeat or mis-index the loop.  The
counter now matches the callback width, and empty or negative lengths are
ignored before indexing the feedback array.

`tests/automated/check-text-preedit-feedback-width.py` protects this behavior.
