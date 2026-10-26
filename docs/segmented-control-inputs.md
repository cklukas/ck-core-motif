---
nav_exclude: true
---

# SegmentedControl state API inputs

The public SegmentedControl item and selection APIs now reject a null widget
before checking the Xt subclass. Invalid optional handles return the existing
neutral values (`False`, `0`, `-1`, or `NULL`) without changing valid item,
selection, callback, or layout behavior.

The focused regression check is
`tests/automated/check-segmented-control-inputs.py`.
