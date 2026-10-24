# Mrm low-level IDB open cleanup

Low-level IDB opens now close the operating-system file descriptor when
allocation of the descriptor or its owned filename fails. This prevents file
descriptor leaks while preserving the existing error results.

The source-level regression is `tests/automated/check-mrm-idb-open-cleanup.py`.
