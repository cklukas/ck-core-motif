---
nav_exclude: true
---

# MWM automation reply initialization

`GetAutomationData` now initializes the output type, buffer, length, and
format before looking up a client window, and returns safely when any output
argument is null. Invalid window or operation requests therefore produce an
empty reply instead of leaving callers with stale output state; valid replies
retain their existing wire format.

The focused regression check is
`tests/automated/check-mwm-automation-reply-initialization.py`.
