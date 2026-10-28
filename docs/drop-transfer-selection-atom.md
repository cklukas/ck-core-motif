# Drop-transfer selection atom

Drop-transfer termination now rejects a null selection pointer or `None` ICC
selection atom before calling Xt selection APIs.  The failure helper also
destroys the transfer object when ICC lookup cannot provide a usable atom.

Regression coverage: `tests/automated/check-drop-transfer-selection-atom.py`.
