---
nav_exclude: true
---

# Mrm text-vector terminators

Char8 and XmString vector writers reserve a null entry so the vector can be
used as an in-memory pointer list.  Both writers now allocate and validate
`count + 1` entries before writing that terminator.  Valid vector contents and
wire representation remain unchanged.
