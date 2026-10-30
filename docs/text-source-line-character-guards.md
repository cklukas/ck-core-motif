---
nav_exclude: true
---
# Unicode text-source line access guards

Unicode line scanning now validates source storage, gap ordering, widget
state, character width, and pointer-offset arithmetic before reading a native
character. Invalid or partially destroyed text-source state returns a neutral
character instead of performing undefined pointer arithmetic.
