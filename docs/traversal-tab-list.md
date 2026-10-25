# Traversal exclusive tab-list allocation

Exclusive tab-list creation, growth, and opportunistic shrinking now publish
replacement arrays only after allocation succeeds.  Failed changes leave the
existing list and capacity intact, avoiding null writes and lost tab entries.

`tests/automated/check-traversal-tablist-allocation.py` protects these paths.
