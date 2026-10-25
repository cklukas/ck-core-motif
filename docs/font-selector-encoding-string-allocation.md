---
nav_exclude: true
---
# FontSelector encoding-label allocation

FontSelector initialization now stops safely if the duplicated encoding-label
string cannot be allocated. This prevents later selector updates from using
missing resource storage while preserving normal label behavior.

`tests/automated/check-font-selector-encoding-string-allocation.py` protects
this contract.
