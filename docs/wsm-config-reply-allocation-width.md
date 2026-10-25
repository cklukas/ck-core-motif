---
nav_exclude: true
---

# WSM configuration-reply allocation width

WSM configuration replies contain three wire-provided attribute arrays. The
decoder now checks each attribute count against Xt's `Cardinal` allocation
width before multiplying by `WSMAttribute` size. Invalid counts use the
existing configuration-clear path, preventing wrapped allocations while
leaving valid replies unchanged.

The regression is covered by
`tests/automated/check-wsm-config-reply-allocation-width.py`.
