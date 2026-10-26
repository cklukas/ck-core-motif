---
nav_exclude: true
---
# TextField initialization storage

TextField resource validation now checks its initial highlight and input-method
storage before initialization. Allocation failure reports through the Xt error
mechanism and returns without dereferencing a null record.

Regression coverage: tests/automated/check-textfield-initial-storage.py.
