---
nav_exclude: true
---
# Accessibility D-Bus property input validation

Unknown or incomplete D-Bus property requests now return without dereferencing
a missing property signature. Known properties retain their existing values and
wire signatures.
