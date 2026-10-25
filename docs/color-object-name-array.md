---
nav_exclude: true
---
# Color-object atom-name allocation

Color-object initialization now checks the screen-count multiplication before
allocating the temporary atom-name pointer array. An unrepresentable product
skips atom interning safely; normal display sizes retain their existing
behavior.

`tests/automated/check-color-object-name-array.py` protects this contract.
