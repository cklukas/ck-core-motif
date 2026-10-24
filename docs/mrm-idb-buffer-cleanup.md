# Mrm in-memory IDB cleanup

When an in-memory UID buffer fails header validation, `UrmIdbOpenBuffer` now
releases the descriptor it allocated before returning the validation error.
Successful opens retain their existing ownership and lifetime behavior.

The source-level regression is `tests/automated/check-mrm-idb-buffer-cleanup.py`.
