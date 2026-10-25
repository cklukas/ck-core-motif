# Empty accessibility cache snapshots

The AT-SPI cache `GetItems` method now treats an empty accessibility tree as a
valid empty result. It avoids calling `XtMalloc(0)` and only reports an
out-of-memory error when a nonempty snapshot allocation fails.

Nonempty snapshots and their failure cleanup are unchanged. The regression is
covered by `tests/automated/check-accessibility-cache-snapshot-allocation.py`.
