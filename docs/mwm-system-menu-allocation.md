---
nav_exclude: true
---

# MWM localized system-menu allocation

The generated localized system-menu copy now checks its final string length
before converting it to Xt's `Cardinal` allocation type. Overlong localized
menu text therefore follows the existing fallback path instead of wrapping the
allocation size.
