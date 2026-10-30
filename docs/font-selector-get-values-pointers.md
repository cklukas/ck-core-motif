---
nav_exclude: true
---

# FontSelector value-hook output pointers

`GetValuesHook` now skips a requested resource whose output pointer is null
before dispatching resource-specific packaging. This protects every supported
FontSelector value branch, while valid `Arg` outputs retain their existing
behavior.

The focused source contract is checked by
`tests/automated/check-font-selector-get-values-pointers.py`.
