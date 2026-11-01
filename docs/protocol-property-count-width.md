---
nav_exclude: true
---

# Protocol-property element-count bounds

The Motif protocol manager can grow beyond its historical fixed protocol
buffer. Its property publisher now also validates that the resulting protocol
count fits Xlib's signed `int` `XChangeProperty` count before allocating or
serializing it. A count outside that transport contract produces the existing
Motif warning and leaves the property unchanged.

Ordinary protocol registration and empty protocol-property updates are
unchanged.
