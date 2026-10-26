---
nav_exclude: true
---

# TreeView model and state API inputs

The public TreeView model, selection, expansion, and top-row APIs now reject a
null widget before invoking the Xt subclass check. Invalid optional handles
return the existing neutral values or no-op result; valid model replacement,
selection callbacks, expansion, and scrolling behavior are unchanged.

The focused regression check is `tests/automated/check-tree-view-inputs.py`.
