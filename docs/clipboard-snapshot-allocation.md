# Check deferred clipboard snapshot allocations

Deferred clipboard snapshots now verify creation of their data-ID dictionary
and per-snapshot request record.  If either allocation fails, the temporary
clipboard copy is ended, buffers are released, and the source reports failure
without publishing callback IDs that cannot be resolved later.

The focused source regression is
`tests/automated/check-clipboard-snapshot-allocation.py`.
