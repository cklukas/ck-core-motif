---
nav_exclude: true
---
# Mrm resource-ID helper inputs

Resource-ID map helpers now reject missing file handles, output handles, and
incomplete map-record allocations before dereferencing them.  Valid RID
lookups and allocation behavior remain unchanged.

The source-level regression is `tests/automated/check-mrm-rid-inputs.py`.
