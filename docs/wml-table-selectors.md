# WML database table selectors

`wmldbcreate` builds the binary table database consumed by the UIL toolchain.
Its internal emitters select one generated table for each table identifier.
Every selector now rejects an unrecognized identifier before using its table
pointer or header. This converts a programming/configuration error into a
clear generator failure instead of allowing an uninitialized header or pointer
to reach `fwrite`.

The source-contract regression is:

```sh
python3 tests/automated/check-wmldb-table-selectors.py
```
