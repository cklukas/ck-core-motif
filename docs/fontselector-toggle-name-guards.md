---
nav_exclude: true
---
# FontSelector name-window callback guards

The name-window callback now tolerates missing callback data, label/family
children, and incomplete font state. Display refreshes occur only when the
needed value is available; valid toggles retain their visibility behavior.
