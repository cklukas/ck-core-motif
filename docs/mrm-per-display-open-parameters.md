---
nav_exclude: true
---

# Mrm public parameter validation

`MrmOpenHierarchyPerDisplay` now rejects a non-null OS-extension list whose
first entry is null before assigning its display.  It returns the established
`MrmFAILURE` result and releases the process lock, matching
`Urm__OpenHierarchy`’s existing invalid-parameter contract.  Valid explicit
and default OS-extension paths are unchanged.

The hierarchy-open and name-registration public entry points also reject
negative and oversized wide-prototype counts before passing them to internal
`MrmCount` (`short`) interfaces.  This prevents an `int` supplied by a
wide-prototype caller from narrowing before the existing validation runs.

The focused regression is
`tests/automated/check-mrm-per-display-open-parameters.py`.
