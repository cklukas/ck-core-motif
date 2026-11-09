---
nav_exclude: true
---
# Mrm hierarchy name API input checks

The hierarchy name-registration and lookup helpers validate the hierarchy
handle before dereferencing it. Registration also rejects null names in a
non-empty batch. Lookup rejects a null output pointer and clears a supplied
output before invalid-hierarchy, invalid-name, local-not-found, or global
fallback failure can return, so callers cannot retain a stale registered value.
Valid registrations and lookups retain their existing hash-table and
global-fallback behavior.
