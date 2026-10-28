---
nav_exclude: true
---
# Clipboard window-tree ownership

`ClipboardSearchForWindow` receives its child-window list from Xlib's
`XQueryTree`. The list is now released with `XFree`, matching the Xlib
allocation contract and avoiding allocator mismatches when clipboard ownership
is checked recursively.
