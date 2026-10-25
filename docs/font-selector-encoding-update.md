---
nav_exclude: true
---
# FontSelector encoding update allocation

FontSelector now builds a replacement encoding label before freeing the old
one. If the replacement allocation fails, the existing label remains valid,
so later selector matching cannot dereference missing state.

`tests/automated/check-font-selector-encoding-update.py` protects this
contract.
