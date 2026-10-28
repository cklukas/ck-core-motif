---
nav_exclude: true
---

# Mrm deferred callback-list sizing

Deferred SetValues callback lists retain a callback descriptor until widget
references resolve.  The retained copy now allocates and copies `count + 1`
items, including the terminator consumed by the resolver, and validates the
descriptor marker before copying.  Valid callback-list behavior is unchanged.
