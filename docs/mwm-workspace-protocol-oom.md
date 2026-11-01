---
nav_exclude: true
---

# MWM workspace-protocol allocation failures

The standalone MWM workspace-protocol bridge now treats allocation failure as
a failed optional protocol operation rather than dereferencing a null result.
Owner tracking verifies the screen-indexed owner table before selecting a
slot. State snapshots omit only their unavailable attribute data, and a failed
nonempty GET_STATE reply allocation leaves the reply unowned and empty.
Configuration-format lookup and initialization also reject unavailable,
incomplete, or unallocatable attribute tables. Incoming version and window-data
lists must have a non-negative count and a matching non-null pointer before
MWM reads them.

Normal workspace protocol replies and ownership monitoring are unchanged.

Run the focused source-contract regression with:

```sh
python3 tests/automated/check-mwm-workspace-protocol-oom.py
```
