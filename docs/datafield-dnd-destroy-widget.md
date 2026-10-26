---
nav_exclude: true
---
# DataField DnD cleanup widgets

DataField DnD destroy cleanup now checks the callback widget before deleting
its drop context. Completion during widget teardown remains safe while the
client-data allocation is still released.

Regression coverage: `tests/automated/check-datafield-dnd-destroy-widget.py`.
