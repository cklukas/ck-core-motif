---
nav_exclude: true
---
# Mrm global name-registration validation

Global MRM name registration now validates every supplied name before it
touches the registry. A null name therefore fails the complete registration
request without registering earlier entries from the same vector. This matches
the existing hierarchy-local registration behavior and preserves all valid
name/value registrations.

The source-level regression is
`tests/automated/check-mrm-wci-hash-callers.py`.
