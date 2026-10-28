---
nav_exclude: true
---

# Mrm child-reference state

`UrmCWRSetChild` now validates the key and appends an index string before
updating the child descriptor. Manage/access flags, key type, and key value are
committed together only after the operation succeeds, so invalid child
references cannot leave partially updated records.
