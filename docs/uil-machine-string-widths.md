---
nav_exclude: true
---

# UIL machine-listing string widths

Machine-listing value and module records now reject index strings that cannot
fit the compact source-record length, and make integer stride and offset-stack
copy widths explicit before passing them to legacy APIs.

Regression: `tests/automated/check-uil-machine-string-widths.py`
