---
nav_exclude: true
---

# Mrm value-fixup pointer inputs

Font-list fixup now validates the file metadata pointer before checking the
database version. Unit-valued integer and floating-point fixup branches also
reject null values before dereferencing them. Valid resource fixups retain
their existing byte-swapping and conversion behavior.
