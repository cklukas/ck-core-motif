---
nav_exclude: true
---

# Mrm legacy callback translation count

Legacy Mrm callback descriptors include a terminating item.  Translation now
allocates and validates space for `count + 1` callback items, matching the
existing inclusive copy loop.  It also rejects descriptors with an invalid
validation marker before reading the count, and the record validator rejects
the same marker for both old and current descriptor formats (including a null
record pointer).  Valid callback descriptors keep their legacy layout and
ownership; malformed or unrepresentable counts are rejected.
