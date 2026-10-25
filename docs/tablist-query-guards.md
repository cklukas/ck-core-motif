# TabList query guards

`XmTabbedStackListQuery` now ignores a `NULL` output structure and malformed
lists before copying fields.  `XmTabbedStackListSimpleQuery` validates its
list and position before using the temporary result, returning `NULL` for an
invalid query instead of reading uninitialized data.

The focused regression is `tests/automated/check-tablist-query-guards.py`.
