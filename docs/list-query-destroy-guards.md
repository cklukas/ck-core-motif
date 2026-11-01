---
nav_exclude: true
---

# XmList query teardown guards

`XmListItemExists`, `XmListItemPos`, `XmListGetKbdItemPos`, and
`XmListGetMatchPos` now return their existing empty/not-found results when a
List is entering Xt teardown. The APIs no longer inspect item, keyboard, or
match storage after child destruction can have begun.

Run the focused source-contract regression with:

```sh
python3 tests/automated/check-list-query-destroy-guards.py
```
