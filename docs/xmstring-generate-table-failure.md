# XmString generation table failure

`XmStringGenerate` and `_XmStringUngenerate` depend on a shared parse table
for tab and newline mappings.  If that table cannot be allocated or
initialized, both APIs now return `NULL` rather than continuing with an empty
table and silently changing the conversion semantics.

The focused source-contract check is:

    python3 tests/automated/check-xmstring-generate-table-failure.py
