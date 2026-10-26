# DataField wide-value initialization

When a resource update supplies no wide value, the legacy DataField setter
creates an empty wide-character value before validation. The allocation is
now checked before writing the terminator or entering validation. An
allocation failure cancels that value change without dereferencing `NULL`.

The regression check is:

```sh
python3 tests/automated/check-datafield-wcvalue-init.py
```
