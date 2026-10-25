---
nav_exclude: true
---

# WSM null request context

The asynchronous WSM receive callbacks now return safely when the toolkit
supplies no pending-request context.  This avoids dereferencing a null
`RequestInfo` before the callback can validate its selection data; normal
requests and callback ownership are unchanged.
