---
nav_exclude: true
---

# WSM empty reply validation

`UTMReplyReceived` now rejects a successful selection conversion with no value
for request types whose reply decoder reads a payload. This prevents the
decoder from dereferencing a null value after a malformed or incomplete
conversion. `WSM_SET_STATE` remains allowed to return an empty payload because
its reply has no encoded fields.

The focused regression check is
`tests/automated/check-wsm-empty-reply-guards.py`.
