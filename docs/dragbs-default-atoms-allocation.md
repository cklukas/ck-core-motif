---
nav_exclude: true
---
# Default drag atom-table allocation handling

`CreateDefaultAtomsTable` creates the initial process-local atom cache.  It
now checks both the table and entry-array allocations, releasing the table if
the entry allocation fails and returning `NULL` instead of dereferencing an
incomplete cache.
