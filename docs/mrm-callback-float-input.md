---
nav_exclude: true
---

# Mrm callback floating-point input

`UrmCWRSetCallbackItem` receives floating-point callback values as pointers.
The writer now rejects a null pointer before appending the callback routine or
touching the callback record. This avoids a null dereference and keeps a failed
write from leaving a partially modified resource context.

The guard applies to `MrmRtypeFloat`, `MrmRtypeHorizontalFloat`, and
`MrmRtypeVerticalFloat`; valid by-address values retain the existing serialized
format and callback behavior.
