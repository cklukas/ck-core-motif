# Mrm empty IDB decommit

Closing an IDB file before any record buffer has been allocated now treats the
empty buffer pool as a successful no-op. This prevents a false decommit failure
from entering the close-error path and deleting or otherwise mishandling a
valid file.

The source-level regression is `tests/automated/check-mrm-idb-empty-decommit.py`.
