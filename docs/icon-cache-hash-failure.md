---
nav_exclude: true
---
# IconFile cache hash and insertion failures

IconFile cache hashing now preserves the full key-name length and handles null
hash keys safely. When a constructed icon-name entry cannot be inserted into
the cache, all owned strings and the entry are released instead of leaking an
unreachable cache record.

The focused regression is `tests/automated/check-icon-cache-hash-failure.py`.
