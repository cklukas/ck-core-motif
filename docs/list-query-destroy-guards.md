---
nav_exclude: true
---

# XmList query teardown guards

`XmListItemExists`, `XmListItemPos`, `XmListGetKbdItemPos`,
`XmListGetMatchPos`, `XmListGetSelectedPos`, `XmListYToPos`,
`XmListPosToBounds`, and `XmListPosSelected` return their established empty,
not-found, or false result when a List is entering Xt teardown. They no longer
inspect item, keyboard, selection, or geometry storage after child destruction
can have begun. `XmListUpdateSelectedList` likewise becomes a no-op instead of
rebuilding selection arrays during teardown.

Run the focused source-contract regression with:

```sh
python3 tests/automated/check-list-query-destroy-guards.py
```
