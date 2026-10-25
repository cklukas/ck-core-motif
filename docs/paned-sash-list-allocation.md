---
nav_exclude: true
---
# Paned sash and separator list allocation

Paned's managed-state update now sizes its temporary sash and separator lists
for every child (the old half-size calculation could under-allocate odd child
counts), and handles empty child sets and allocation failure before populating
them. Any partially allocated lists are released and the update returns
without changing valid layout behavior.

`tests/automated/check-paned-sash-list-allocation.py` protects this contract.
