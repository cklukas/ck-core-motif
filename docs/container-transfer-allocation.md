---
nav_exclude: true
---
# Container transfer allocation

Container primary-transfer actions keep a temporary event context, and
copy/link/move and drop callbacks keep temporary pointer coordinates.  All of
these allocations are now checked.  An allocation failure abandons only the
current transfer setup or callback, leaving the widget and existing transfer
state valid instead of dereferencing a null pointer.

`tests/automated/check-container-transfer-allocation.py` protects this
contract.
