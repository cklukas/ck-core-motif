# Exm Container constructor declaration

The historical unsupported Exm Container sample now declares
`ExmCreateContainer` with the same `Widget`, name, argument-list, and argument
count parameters as its implementation.  This lets current C compilers check
sample callers instead of treating the constructor as an unprototyped function.

The focused source-contract check is:

```sh
python3 tests/automated/check-exm-container-c23-api.py
```
