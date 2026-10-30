---
nav_exclude: true
---
# DropDown popup lifecycle

DropDown now checks popup-shell creation before constructing its scrolled list,
and checks list creation before registering callbacks or scroll-bar handlers.
Partial construction is destroyed cleanly when either allocation fails.

The popup shell also clears the stored shell, list, and scrollbar pointers when
destroyed externally, so later popup operations cannot reuse stale widgets.
