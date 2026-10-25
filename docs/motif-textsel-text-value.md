# Text encoding value conversion failure

Text and encoding conversion now rejects a null source string before measuring
its length, keeping a failed source allocation from becoming a null dereference.

The regression check is
`tests/automated/check-textsel-text-value.py`.
