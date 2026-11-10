---
nav_exclude: true
---
# EditRes fallback formatting

The edit-resources protocol converts widget resources to strings. When a
resource converter fails, the integer fallback appends the resource type to a
caller-supplied buffer. Resource type names are application-defined and can be
long.

`GetResourceString` receives the capacity of its buffer, formats numeric
values with `snprintf` and appends the type within the remaining space. The
value is read into storage of the size the resource declares, so the
conversion sees the bytes of the right width on any byte order.

`tests/automated/check-editres-fallback-bounds.py` guards the interface and
formatting contracts.
