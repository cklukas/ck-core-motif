---
nav_exclude: true
---
# DataField DnD drag context

DataField drag-motion and drop callbacks now reject callback payloads without
a drag context before passing that context to Xt or Motif transfer APIs.

Regression coverage: `tests/automated/check-datafield-dnd-context.py`.
