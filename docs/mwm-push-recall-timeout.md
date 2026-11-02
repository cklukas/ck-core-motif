---
nav_exclude: true
---

# MWM push/recall launch timeout

Front-panel push/recall actions may use the optional busy-light widget's
`XmNclientTimeoutInterval` resource to suppress repeated launch requests while
the requested client starts. MWM initializes that interval to zero and creates
a deadline only when the widget supplies a positive value. A missing panel,
busy indicator, zero, or negative resource value therefore leaves no stale or
uninitialized deadline and does not suppress a later launch request.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-push-recall-timeout.py
```
