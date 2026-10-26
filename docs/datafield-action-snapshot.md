# DataField action event snapshots

The DataField `df_Stuff` action now checks both its action-record and copied
event allocations before use. A missing event or allocation failure exits
cleanly without dereferencing or leaking a partial snapshot.

The regression check is:

```sh
python3 tests/automated/check-datafield-action-snapshot.py
```
