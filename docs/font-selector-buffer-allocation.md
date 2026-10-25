---
nav_exclude: true
---
# FontSelector reusable buffer allocation

FontSelector initialization now stops safely when its reusable font-formatting
buffer cannot be allocated. This prevents later font-string construction from
writing through a null buffer while preserving normal initialization.

`tests/automated/check-font-selector-buffer-allocation.py` protects this
contract.
