# XmString byte-stream conversion locking

`XmCvtByteStreamToXmString` now delegates synchronization to its cache and
representation helpers instead of holding the process lock across conversion.
This prevents recursive locking while preserving the existing validation and
conversion behavior.

The source contract is covered by
`tests/automated/check-xmstring-byte-conversion-locking.py`.
