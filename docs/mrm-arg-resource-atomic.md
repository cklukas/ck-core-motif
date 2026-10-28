---
nav_exclude: true
---

# Mrm argument resource-reference state

`UrmCWRSetArgResourceRef` now assigns the argument’s resource representation
only after `UrmCWR__AppendResource` succeeds and the argument pointers have
been rebound. A failed resource append therefore cannot leave a partially
updated argument descriptor.
