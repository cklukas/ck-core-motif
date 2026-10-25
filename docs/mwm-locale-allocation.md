---
nav_exclude: true
---

# MWM locale-copy allocation

The MWM configuration loader now checks the locale string length before
converting it to Xt's `Cardinal` allocation size. An unrepresentable locale
value is treated like an allocation failure instead of wrapping the requested
size.
