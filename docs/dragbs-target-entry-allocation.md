---
nav_exclude: true
---
# Drag target-entry allocation handling

`ReadTargetsTable` allocates an atom array for each newly discovered target
list.  If one allocation fails, the reader now rejects the update and rolls
back entries added during that update, freeing their atom arrays and restoring
the previous entry count.  This avoids writing through a null array and keeps
the existing cache coherent after low-memory failures.
