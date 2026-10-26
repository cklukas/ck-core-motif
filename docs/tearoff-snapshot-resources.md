---
nav_exclude: true
---
# Tear-off snapshot resources

When a menu is torn off, Motif snapshots the submenu into a background
pixmap.  The snapshot path now checks the temporary graphics context and
background pixmap before copying.  If either resource cannot be created, the
operation returns after releasing the GC instead of issuing an invalid copy or
publishing a `None` background pixmap.
