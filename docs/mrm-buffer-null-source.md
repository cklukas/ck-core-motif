---
nav_exclude: true
---
# Mrm in-memory record source validation

In-memory record reads allow the header record to bypass the record-number upper
bound because the header is needed to establish file metadata. That exception
must not bypass the source-buffer check: a malformed descriptor with a null UID
buffer would otherwise copy from address zero. The null check now applies to all
in-memory records while the header-number exception remains intact.
