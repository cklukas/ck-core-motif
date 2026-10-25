# Window-state placement cleanup guards

State cleanup now validates workspace records, placement storage, and stored
icon indices before clearing entries in WSM and non-WSM paths.  Valid cleanup
continues to remove the client from its placement slot.

`tests/automated/check-winstate-removal-place-guards.py` protects the contract.
