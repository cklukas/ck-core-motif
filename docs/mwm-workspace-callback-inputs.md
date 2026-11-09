---
nav_exclude: true
---

# MWM workspace callback input lifecycle

The standalone MWM workspace protocol runs during startup, selection-owner
changes, and asynchronous reply delivery.  Those paths can see a missing
request/reply object, an unavailable screen widget, or a client whose screen
is already being detached.

Its public callback entry points now validate those values before querying Xt
or Xlib.  A malformed conversion request with an available reply receives an
empty `WSM_UNKNOWN` result; a missing reply, client, screen, or UTM shell is a
no-op.  A pending owner-monitor timer stops instead of rescheduling through a
missing UTM shell.  Valid workspace messages retain their existing protocol
contents and scheduling behavior.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-workspace-callback-inputs.py
```
