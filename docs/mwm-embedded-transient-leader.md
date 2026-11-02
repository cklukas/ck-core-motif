---
nav_exclude: true
---

# MWM embedded transient leader fallback

When an embedded front-panel client owns a transient, MWM may replace the
embedded client with the front-panel client as the transient leader. The
replacement now occurs only after its X context lookup returns a real panel
client. During panel creation or teardown, a failed lookup therefore retains
the valid leader already resolved from the client hint instead of turning the
transient into an unrelated top-level window.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-embedded-transient-leader.py
```
