---
nav_exclude: true
---
# FontSelector encoding-list allocation

FontSelector now duplicates its encoding list atomically. A failed list or
entry allocation releases any partial copy and leaves the original resource
list in place for cleanup, avoiding a partially initialized widget list.

`tests/automated/check-font-selector-encoding-list-allocation.py` protects this
contract.
