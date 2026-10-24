# Mrm header data-entry bounds

Header data-entry reads and writes now validate file/context/output handles,
header record type, item offsets, free-space metadata, and entry sizes before
accessing or extending the fixed header record.  Simple-entry sizes use
`MrmCount` arithmetic, avoiding truncation on large or malformed inputs.

The source-level regression is
`tests/automated/check-mrm-header-data-bounds.py`.
