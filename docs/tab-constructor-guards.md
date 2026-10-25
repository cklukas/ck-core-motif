---
nav_exclude: true
---
# Tab-list widget constructor guards

The private tab-list widget constructors now return `NULL` for a missing
parent or failed list allocation. `_XmCreateTab` also stops before accessing a
null parent list. Existing successful construction and resource handling are
unchanged.
