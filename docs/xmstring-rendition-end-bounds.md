---
nav_exclude: true
---
# XmString rendition-end tag bounds

Compound-string rendering removes rendition-end tags by scanning the active
tag list in reverse. The tag count is unsigned, so the scan now counts down
without narrowing `tag_count - 1` into a signed loop index. Normal rendition
ordering and end-tag removal are unchanged, while large counts no longer risk
implementation-defined conversion or loop underflow.

The source contract is checked by
`tests/automated/check-xmstring-rendition-end-bounds.py`.
