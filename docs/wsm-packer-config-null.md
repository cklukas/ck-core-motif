---
nav_exclude: true
---

# WSM packer configuration failure

Request packing now checks all three configuration-format records before
reading their counts.  If display/screen metadata could not be created, the
packer returns `NULL` with `WSM_ERROR_INTERNAL` rather than dereferencing a
missing record.  Attribute lookup likewise returns no match for null or
internally inconsistent format records, preserving safe failure behavior for
callers.
