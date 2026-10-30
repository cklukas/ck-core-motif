---
nav_exclude: true
---

# SelectionBox synthetic getter guards

SelectionBox exposes several compound-string and list resources through
synthetic `XtGetValues` entries. Their callbacks now validate both the output
`XtArgVal *` and the source widget before touching SelectionBox instance data.
Calls made with a null or unrelated widget return a deterministic null/zero
value instead of dereferencing invalid state. Existing child-widget behavior
and public resource names are unchanged.

The source-contract regression check is:

```sh
python3 tests/automated/check-selectionbox-synthetic-getter-guards.py
```
