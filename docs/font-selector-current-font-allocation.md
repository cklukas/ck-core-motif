---
nav_exclude: true
---
# FontSelector current-font allocation

FontSelector initialization now stops before display setup when its current
`FontData` record cannot be allocated. This avoids dereferencing an incomplete
font record while preserving normal font selection behavior.

`tests/automated/check-font-selector-current-font-allocation.py` protects this
contract.
