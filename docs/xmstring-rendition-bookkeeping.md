---
nav_exclude: true
---
# XmString active-rendition bookkeeping

Active rendition updates now reject null source lists and inconsistent context
storage. Failed bookkeeping marks the context failed instead of copying from
or traversing null pointers.

The regression check is
`tests/automated/check-xmstring-rendition-bookkeeping.py`.
