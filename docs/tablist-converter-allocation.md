---
nav_exclude: true
---
# Tab-list resource conversion allocation

The tab-list resource converter now checks each `XmTabCreate` result. A failed
tab allocation releases any partial tab list and returns `False` instead of
passing a null tab into `XmTabListInsertTabs`.
