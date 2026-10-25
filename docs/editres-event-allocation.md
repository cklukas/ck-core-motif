---
nav_exclude: true
---
# Editres event allocation

Editres protocol parsing now checks the event record and SetValues payload
allocations before dereferencing them. Resource failure or malformed requests
take the existing cleanup path instead of writing through null storage.

`tests/automated/check-editres-event-allocation.py` protects this contract.
