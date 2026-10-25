---
nav_exclude: true
---
# Transient list guards

Transient windows can be withdrawn while their workspace or parent tree is
being detached.  `AddTransient` now validates workspace, screen, leader, and
tree-top state before linking or restacking.  `DeleteTransient` safely ignores
an already-detached client.  Existing modal-count and stacking behavior is
unchanged for valid transient trees.
