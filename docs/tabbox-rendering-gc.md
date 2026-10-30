---
nav_exclude: true
---

# TabBox rendering graphics context

`XmTabBox` may create its drawing graphics context lazily during redisplay.
Redisplay now stops safely when that allocation fails and releases any damage
region it created, rather than entering tab drawing with a null GC.

The focused source contract is checked by
`tests/automated/check-tabbox-render-gc.py`.
