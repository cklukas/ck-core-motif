---
nav_exclude: true
---
# Table converter destructor payloads

The `XmString` and plain-string table converter destructors now tolerate a
null table payload after receiving a valid `XrmValue`. This keeps cleanup safe
when conversion setup or an embedding caller supplies an empty result, while
leaving normal table ownership and freeing behavior unchanged.

`tests/automated/check-table-destructor-pointers.py` protects this contract.
