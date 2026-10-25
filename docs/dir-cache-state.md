---
nav_exclude: true
---
# Directory-cache backing state

The file-selector directory cache now rejects insertions when its backing
directory string is unavailable, such as after a failed cache reset.  This
prevents a null write while preserving normal cache population and lookup.

`tests/automated/check-dir-cache-state.py` protects this contract.
