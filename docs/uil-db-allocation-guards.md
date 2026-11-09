---
nav_exclude: true
---

# UIL database allocation guards

The UIL compiler rejects a malformed or oversized database cleanly when a
nonempty allocation for table pointers, length records, strings, or enum
values fails.  It no longer passes a null allocation result into the database
reader, and releases temporary length records when later string allocation
fails.

Empty serialized sections are still accepted: no pointer is required when the
corresponding read has zero bytes and no consumer can dereference it.
