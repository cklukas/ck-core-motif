# Drag atom-table allocation handling

`ReadAtomsTable` lazily creates and grows the process-local atom cache.  The
reader now checks both the lazy table allocation and the entry-array
reallocation before parsing entries.  A failed allocation leaves the existing
cache untouched and rejects the update instead of dereferencing or replacing
it with a null pointer.
