---
nav_exclude: true
---

# Mrm color-table arguments

Color-table arguments now use their validated entry count when calculating
serialized storage, reject invalid or oversized tables, and copy the complete
table into the resource record before storing its offset. This prevents the
previous fixed-size allocation from omitting entries and avoids leaving the
record uninitialized.
