---
nav_exclude: true
---

# Editres widget-array allocation

Editres protocol parsing allocates widget-information arrays for SetValues,
FindChild, geometry/resource queries, and GetValues requests. Each allocation
is now checked before the parser stores entries into the array. Zero-entry
GetValues requests are rejected because the command consumes a first widget;
other zero-entry arrays remain valid even when the allocator returns `NULL`.
Non-zero requests follow the existing `done` cleanup path when storage is
unavailable.

The focused regression check is
`tests/automated/check-editres-widget-array-allocation.py`.
