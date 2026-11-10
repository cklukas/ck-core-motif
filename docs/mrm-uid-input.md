# UID files as untrusted input

A UID file is read by the resource manager (`MrmOpenHierarchyPerDisplay`,
`MrmFetch...`) and everything in it comes from outside the process: offsets,
sizes, counts and strings. This page lists what the loader now checks that it
used to trust, and how it is tested.

## What is checked

- **Index lookups.** A name that matches an entry of an interior node of the
  index tree used to return the match with no record, which every lookup then
  dereferenced. Lookups of names stored in interior nodes, which is most of them
  in an interface of any size, now return the node's entry. (`Idb__INX_FindIndex`)
- **Literals.** A literal whose value is a number or a C string must contain the
  bytes of that value: an integer, boolean or single float has four bytes, a
  float eight, and a string, keysym, translation table, class record name or
  bitmap file name ends inside the entry. An entry with no data is refused
  instead of being given a null buffer. (`UrmIdbGetIndexedResource`,
  `UrmIdbGetRIDResource`)
- **Widget arguments.** An argument whose value is a string stored in the widget
  record (a plain string, a translation table, a class record name, a keysym)
  must end inside the record.
- **Icon images.** The icon, its pixel data and its color table must lie in the
  buffer they are read from, the pixel data must cover width times height
  pixels at the stated depth, the color table must have the foreground and
  background entries and as many entries as it claims, and color descriptors and
  resource descriptors in it must lie in the buffer and end their names inside
  it. `Urm__CW_LoadIconImage` takes the size of that buffer.
- **Compression tables.** The class and resource name tables are checked against
  their own size: the magic number, the entry count, the offset of every string
  and the end of every string. A table that is not valid is dropped when the
  hierarchy is opened instead of being used. `Urm__FixupCompressionTable` takes
  the size of the table.
- A zero-length data entry no longer copies into a missing buffer.

## Testing

`tests/automated/run-mrm-uid-mutations.py PREFIX RESULTS` compiles two
interfaces with the UIL compiler of the installed toolkit
(`tests/automated/mrm-uid-corpus.uil`, which defines one exported value of most
literal kinds, and `demos/programs/periodic/periodic.uil`), damages the UID
file in a reproducible way (byte changes, extreme values in 16 and 32 bit
fields, truncation, copied blocks, runs of one byte; the copy number selects
the damage) and loads every copy: it opens the hierarchy and fetches the
widgets, literals, colors, icons and bitmaps the interface defines.
`tests/automated/check-mrm-uid-mutations.c` runs every copy in a child process
under AddressSanitizer and UndefinedBehaviorSanitizer and attributes a failure
to the first frame of toolkit code in the report: a failure that first enters the
resource manager fails the test, one that first enters widget code (a damaged
file can give a widget resource a value of the wrong type) is reported as a note.
The unchanged file must load completely, which also exercises the index fix; the
earlier loader crashes on it.
