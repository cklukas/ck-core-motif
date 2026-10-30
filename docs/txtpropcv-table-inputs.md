---
nav_exclude: true
---

# Text-property export table inputs

All non-empty XmString tables passed to
`XmCvtXmStringTableToTextProperty` must contain valid entries. The exporter
now rejects a NULL entry with `BadValue` before any format-specific conversion,
avoiding silent empty elements and keeping all encoding styles consistent.

The focused regression check is
`tests/automated/check-txtpropcv-table-inputs.py`.
