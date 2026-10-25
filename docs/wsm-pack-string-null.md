---
nav_exclude: true
---

# WSM string packing

`PackString` now treats a null input as an empty string before measuring and
packing it.  This protects extension and application-supplied protocol data
from a null-pointer `strlen` while retaining the existing length-prefixed wire
format.
