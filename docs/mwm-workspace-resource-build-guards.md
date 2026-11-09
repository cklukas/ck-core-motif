---
nav_exclude: true
---

# MWM workspace-resource build guards

Workspace resource tables only exist in MWM builds that include workspace
management. Non-WSM builds now skip those lookups entirely instead of applying
`XtNumber` to their null placeholder pointers. This preserves the WSM resource
behavior and prevents incorrect pointer-size counts in ordinary MWM builds.

```sh
python3 tests/automated/check-mwm-workspace-resource-build-guards.py
```
