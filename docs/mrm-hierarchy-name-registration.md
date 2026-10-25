---
nav_exclude: true
---
# Mrm hierarchy name registration

Hierarchy-local name registration now validates non-empty name/value arrays,
checks allocation of the hash table, and stops cleanly when a hash entry cannot
be created. Valid registrations retain their existing name-to-value mappings.

The source-level regression is
`tests/automated/check-mrm-hierarchy-name-registration.py`.
