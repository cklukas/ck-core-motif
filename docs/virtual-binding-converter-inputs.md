---
nav_exclude: true
---
# Virtual-binding converter input validation

The internal Xt converter for virtual-key binding resources now rejects a
missing display, source value, source string, or destination value before it
accesses converter state. Valid resource conversion behavior is unchanged;
invalid direct-converter input now fails normally instead of dereferencing a
null pointer.

The focused regression check is
`tests/automated/check-virtual-binding-converter-inputs.py`.
