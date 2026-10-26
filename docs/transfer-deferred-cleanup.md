# Deferred clipboard target cleanup

Deferred clipboard target lists now have explicit ownership handling. A
successful but unusable list is released, a valid list transfers ownership to
the request loop, and a successful snapshot conversion must provide a value
before it is dereferenced. This closes leak and null-dereference paths during
deferred clipboard transfers.

The regression check is:

```sh
python3 tests/automated/check-transfer-deferred-cleanup.py
```
