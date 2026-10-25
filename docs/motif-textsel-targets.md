# Text TARGETS allocation failure

Text TARGETS conversion now rejects a failed standard-target list allocation
before writing additional atoms into the returned array.

The regression check is
`tests/automated/check-textsel-targets-allocation.py`.
