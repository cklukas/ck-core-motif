---
nav_exclude: true
---
# DataField modify-verify cleanup

DataField now validates the wide modify-verify callback's text record and
payload before reading it. Invalid callback data follows the existing cleanup
label, releasing any temporary narrow or wide conversion buffers instead of
returning early with leaked state or dereferencing a null record.

Regression coverage: `tests/automated/check-datafield-modify-verify-cleanup.py`.
