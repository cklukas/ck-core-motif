# Mrm hash lookup and deletion inputs

Hash lookup and deletion now reject null tables or names before indexing or
measuring them. Both helpers also reject names that cannot be represented by
the legacy hash function’s `int` length parameter. Valid lookup and deletion
ordering remains unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-hash-helpers.py`.
