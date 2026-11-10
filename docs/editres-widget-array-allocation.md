---
nav_exclude: true
---

# Editres widget-array allocation

Editres protocol parsing allocates one widget path per entry of SetValues,
geometry and resource queries. The announced entry count is compared with the
bytes left in the request (every path takes at least six) before the array is
allocated, and each path is checked in turn; a request that fails half way
releases the paths it already read. FindChild and GetValues read a single path.
Zero-entry GetValues requests are rejected because the command consumes a
first widget; other zero-entry arrays remain valid.

The focused regression check is
`tests/automated/check-editres-widget-array-allocation.py`.
