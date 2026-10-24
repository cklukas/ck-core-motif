# Mrm pointer-list resize limits

`UrmPlistResize` now rejects a requested slot count below the number of active
pointers. This prevents callers from truncating live entries and leaving the
list's ownership and indexing state inconsistent.
