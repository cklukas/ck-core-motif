---
nav_exclude: true
---

# FontSelector SetValues resource names

`SetValues` now preflights every requested resource name before passing the
argument list to `_XmFilterArgs`. A nameless `Arg` returns `False` without
entering child-resource filtering; valid resource updates are unchanged.

The focused source contract is checked by
`tests/automated/check-font-selector-set-values-names.py`.
