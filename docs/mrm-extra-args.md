# Mrm extra-argument counts

Mrm extra-argument metadata now rejects counts above the signed record limit
before storing them in the descriptor's `MrmCount` field. This prevents large
`Cardinal` inputs from wrapping negative and preserves valid runtime arg-list
behavior.

The source-level regression is `tests/automated/check-mrm-extra-args.py`.
