---
nav_exclude: true
---

# Clipboard format enumeration null-name handling

`ClipboardFindFormat` supports both named matching and name-less enumeration.
The latter is used by `XmClipboardInquireCount`; it now skips `XInternAtom`
when no name is supplied, avoiding a null atom-name request while preserving
the existing enumeration behavior.

The focused regression check is
`tests/automated/check-clipboard-format-enumeration-null-name.py`.
