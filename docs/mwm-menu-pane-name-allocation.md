---
nav_exclude: true
---

# MWM menu-pane name allocation

Resource parsing now measures a menu-pane name before copying it into the
`MenuSpec`. Names that cannot fit Xt's `Cardinal` allocation width follow the
existing parser-warning and cleanup path instead of wrapping the allocation
size. Normal menu names and resource syntax are unchanged.
