---
nav_exclude: true
---
# XmString generated parse-table publication

The parse mappings used by `XmStringGenerate` and the internal reverse helper
are now built in a private candidate table. The shared pointer is published
only after both mappings and their substitutes have been validated. This
preserves the existing lazy initialization and API behavior while preventing a
concurrent caller from using a partially initialized table.

The regression check is
`tests/automated/check-xmstring-generated-table-publication.py`.
