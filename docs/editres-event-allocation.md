---
nav_exclude: true
---
# Editres event allocation

Editres protocol parsing checks the event record and the SetValues value
allocations before using them. Resource failure or malformed requests take
the common cleanup path, which now releases every part of the request,
including the value and the names of requests that were not SetValues.

`tests/automated/check-editres-event-allocation.py` protects this contract.
