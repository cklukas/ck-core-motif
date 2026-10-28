---
nav_exclude: true
---
# Clipboard atom-name ownership

`XmClipboardInquireFormat` receives format names from `XGetAtomName`. Those
strings are Xlib-owned and must be released with `XFree`; using `XtFree` can
trigger allocator mismatches in applications that use separate Xt and Xlib
allocation hooks. The query now preserves the Xlib ownership contract.
