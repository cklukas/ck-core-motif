---
nav_exclude: true
---
# List add input validation

The List add APIs now reject null widgets and null single-item strings before
entering the application context. Shared insertion also rejects non-positive
counts and null elements in an item array, preventing invalid calls from
reaching list mutation or string-copy code.

`tests/automated/check-list-add-inputs.py` protects this contract.
