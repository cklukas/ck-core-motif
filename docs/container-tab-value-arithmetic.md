---
nav_exclude: true
---

# Container tab-value arithmetic

Container detail tabs convert the dimension and column index to `float`
before multiplication. This avoids the old `int`-by-`Cardinal` promotion,
which could wrap a large detail count before `XmTabSetValue` received it.
