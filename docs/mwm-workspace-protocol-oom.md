---
nav_exclude: true
---

# MWM workspace-protocol allocation failures

The standalone MWM workspace-protocol bridge now treats allocation failure as
a failed optional protocol operation rather than dereferencing a null result.
Owner tracking verifies the screen-indexed owner table before selecting a
slot. State snapshots omit only their unavailable attribute data, and a failed
nonempty GET_STATE reply allocation leaves the reply unowned and empty.

Normal workspace protocol replies and ownership monitoring are unchanged.

Run the focused source-contract regression with:

```sh
python3 tests/automated/check-mwm-workspace-protocol-oom.py
```
