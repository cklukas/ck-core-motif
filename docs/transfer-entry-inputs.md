---
nav_exclude: true
---

# Transfer source and sink entry inputs

Selection and drag-transfer entry points now reject null widget handles before
application-context or display lookup. Named sinks also reject an absent
selection atom, and standard conversion rejects a missing callback structure.
Valid source, sink, drag, and conversion behavior is unchanged.

The focused regression check is
`tests/automated/check-transfer-entry-inputs.py`.
