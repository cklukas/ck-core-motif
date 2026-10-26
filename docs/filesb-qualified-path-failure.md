# File selection qualified-path failure handling

The File Selection Box callback now checks the results of qualified path
construction before measuring or copying them.  Allocation or input-size
failure leaves the callback result fields in their initialized null state and
returns cleanly instead of dereferencing a null path.

The focused source-contract check is:

    python3 tests/automated/check-filesb-qualified-path-failure.py
