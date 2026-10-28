---
nav_exclude: true
---

# Native List horizontal-scroll regression

`tests/automated/run-list-horizontal-scroll.py` builds an installed-library
fixture and runs it under Xvfb. The fixture scrolls long List content to the
far right, replaces it with short content, changes the margin, and verifies
that the horizontal scrollbar keeps a positive slider/increment/page size and
an in-range value.
