---
nav_exclude: true
---
# XmList copy-helper rollback

The `XmList` initialization and resource-update paths copy caller-provided
item, selected-item, and selected-position arrays into widget-owned storage.
The item helpers now release every earlier `XmString` copy if a later copy
fails, clear the failed resource, and reset its count instead of publishing a
partially initialized table.  Selected-position byte counts are checked both
against host `size_t` and Xt's `Cardinal` allocation range before conversion.
All three copy-helper byte counts are passed explicitly as Xt `Cardinal`
values after those checks.

The focused regression is
`tests/automated/check-list-copy-helper-rollback.py`.
