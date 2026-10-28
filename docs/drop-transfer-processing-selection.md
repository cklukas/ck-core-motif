# Drop-transfer processing selection validation

Drop-transfer processing now rejects a missing ICC selection atom before it
registers an Xt selection request. `None` is not a valid selection owner for
the `XtGetSelectionValue*` APIs; attempting to use it could leave a transfer
marked pending without a request that can complete. The existing failure path
cleans up temporary target arrays and terminates or destroys the transfer as
appropriate. Valid external-owner transfers are unchanged.

Regression coverage: `tests/automated/check-drop-transfer-processing-selection.py`.
