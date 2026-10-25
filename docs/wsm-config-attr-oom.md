# WSM configuration attribute allocation failure

Configuration-format reply unpacking now checks each screen attribute-list
allocation. If one fails, it clears the failed count, releases any earlier
lists through the existing screen cleanup helper, and returns before writing
through a null pointer. Successful replies retain the existing cache and
reply-pointer behavior.

`tests/automated/check-wsm-config-attr-oom.py` protects this rollback path.
