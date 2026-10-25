---
nav_exclude: true
---

# WSM null transfer data

The asynchronous WSM reply callback now handles a null selection callback
structure as an internal transfer error.  It notifies an available reply
callback and releases the pending request record, preventing a null-pointer
deref and stale request leak when the toolkit supplies malformed callback
data.
