# Mrm file seek checks

Mrm block reads and writes now check the `lseek` result before issuing the
system I/O call. A failed seek returns `MrmFAILURE` without reading or writing
at an unintended file position; valid record-number I/O is unchanged.

The regression extends `tests/automated/check-mrm-file-io-input.py`.
