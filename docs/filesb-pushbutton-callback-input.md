---
nav_exclude: true
---

# FileSelectionBox push-button callback input

`FileSelectionPB` now rejects missing callback widgets, callback data, and
parent FileSelectionBox widgets before accessing dialog state. It also keeps
mask and directory lengths at zero if compound-string generation fails.
Normal Apply/OK/Cancel button behavior is unchanged.

The focused source contract is checked by
`tests/automated/check-filesb-pushbutton-callback-input.py`.
