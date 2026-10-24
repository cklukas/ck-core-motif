# Mrm IDB decommit cleanup

`Idb__BM_DecommitAll` now attempts every buffer belonging to a file, records
the first write error, and detaches each buffer before returning. Failed
buffers are marked inactive and unmodified so a later close cannot retain a
dangling file pointer after the file handle is released.

The source-level regression is `tests/automated/check-mrm-idb-decommit-cleanup.py`.
