---
nav_exclude: true
---

# Compound-text conversion failures

When importing a COMPOUND_TEXT property, every bounded segment must convert to
an `XmString`. A failed segment conversion now releases previously converted
entries and rejects the entire table instead of returning success with a NULL
entry. This keeps the public result internally valid for callers that iterate
the returned string table.

The focused regression check is
`tests/automated/check-compound-text-conversion-failure.py`.
