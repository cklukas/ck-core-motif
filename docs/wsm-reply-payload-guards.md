---
nav_exclude: true
---

# WSM reply payload validation

`UTMReplyReceived` now rejects a selection reply that reports a non-zero
length with a null value before passing it to `_WSMUnpackReply`. The requester
receives `WSM_ERROR_INTERNAL`, and valid replies retain their existing callback
and wire behavior.

The focused regression check is
`tests/automated/check-wsm-reply-payload-guards.py`.
