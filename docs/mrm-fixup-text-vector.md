---
nav_exclude: true
---

# Mrm text-vector fixup guards

Text-vector fixup now rejects a null value buffer, null record buffer, or null
byte-swap flag before dereferencing them.  It also rejects negative vector
counts instead of treating malformed vectors as empty.  Valid Char8 and
XmString vectors retain their existing fixup and conversion behavior.
