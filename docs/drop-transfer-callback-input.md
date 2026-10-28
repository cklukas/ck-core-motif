# Drop-transfer callback input

Drop-transfer processing now requires a transfer callback before issuing a
selection request.  A missing `XmNtransferProc` fails the transfer through the
normal cleanup path instead of later dereferencing a null callback pointer.

Regression coverage: `tests/automated/check-drop-transfer-callback-input.py`.
