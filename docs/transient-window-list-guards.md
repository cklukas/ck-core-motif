---
nav_exclude: true
---
# Transient window-list guards

Restacking builds a temporary window list from the transient tree.  The list
builder now returns its current output pointer when the output buffer or
client tree is missing, avoiding writes through a null buffer during failed
allocation or teardown.  Valid list construction is unchanged.
