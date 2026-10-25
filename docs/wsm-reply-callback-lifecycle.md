---
nav_exclude: true
---

# WSM reply callback lifecycle

WSM reply callbacks are optional.  The receive path now checks for a null
callback before invoking it on either success or conversion failure.  If the
selection transfer callback reports malformed status, the pending request
record is released and an available callback receives an internal-error
notification instead of leaking the request state.
