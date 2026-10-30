---
nav_exclude: true
---
# Container DnD placement callback guards

`MoveItemCallback` now rejects missing callback data, transfer value data, and
child Container constraints before adjusting the drop point or placing the
item.  Valid drag-and-drop placement is unchanged; malformed or teardown-time
callbacks now return safely.

The source contract is checked by
`tests/automated/check-container-move-item-callback-input-guards.py`.
