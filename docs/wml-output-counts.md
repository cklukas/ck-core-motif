# WML database output counts

`wmldbcreate` serializes generated tables whose legacy metadata stores sizes
and counts as signed integers. Before passing those values to `fwrite`, the
generator now verifies that they are non-negative and converts them through one
checked helper. It also rejects a negative enum-set value count and validates
the bit-vector count before rounding it up.

This prevents a malformed generated input from converting a negative legacy
count into a huge `size_t` write length. Valid generated databases retain their
existing byte layout.

The key-table and enum-set record formats retain their historical pointer-sized
fields so existing readers continue to receive the same record layout. Their
on-disk pointer slots are now zeroed in temporary serialization copies. UIL
readers reconstruct `at_name` pointers from the following string payload and
enum-set value pointers from their following value payload, so storing live
process addresses was both unnecessary and made identical databases vary with
address-space randomization.

The focused source-contract regression is:

```sh
python3 tests/automated/check-wmldb-output-counts.py
```
