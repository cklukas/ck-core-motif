---
nav_exclude: true
---

# Client-window conversion safety

The standard transfer converter now handles a widget with no shell ancestor
when serving the client-window target. It frees the temporary result and
reports an unavailable conversion instead of dereferencing a null ancestor
during teardown or unusual widget hierarchies.

The focused regression check is
`tests/automated/check-transfer-client-window.py`.
