---
nav_exclude: true
---

# MWM backdrop request diagnostics

A CDE backdrop-change request must use the exact request token, followed by the
end of the request or a space-separated argument. Prefixes of unrelated request
names are ignored instead of being mistaken for backdrop changes.

A valid backdrop-change request without a path reports only the missing-path
error. MWM reports allocation failure only when a present path cannot be copied.

```sh
python3 tests/automated/check-mwm-backdrop-request-diagnostics.py
```
