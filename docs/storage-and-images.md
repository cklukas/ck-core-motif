---
nav_exclude: true
---
# Clipboard storage and image input

## Native selection storage

X selection formats 8, 16 and 32 count logical items. Xlib stores a format-32
item in a native `long`, including on LP64 platforms where that occupies eight
bytes. Property offsets and `bytes_after` still use the X protocol's four-byte
units and wire bytes. Clipboard assembly therefore sizes each received chunk in
native units and advances the property offset in wire units. These counts must
not be mixed when a property crosses a request boundary.

`XmClipboardCopy` and `XmClipboardRetrieve` retain their existing byte-length
API and native-long representation for format 32. Only the low 32 bits travel
through X; Xlib may sign-extend the returned native longs. Stored data is
independent of its input buffer. A changed property type/format or a native
allocation exceeding Xt's `Cardinal` limit fails assembly and releases partial
storage. This preserves the historical property representation and public ABI.

`XmeConvertMerge` accepts matching formats 8, 16 and 32. It bounds both item
counts before computing native byte sizes. Invalid or unrepresentable input
emits the normal conversion warning and preserves the existing value/count.
An empty append is a no-op; appending data from the existing value is supported.

Unified transfer owners retain one destroy callback per widget/selection,
including after repeated acquisition. The callback releases the selection atom
from its closure. The pinned Xt dependency patch additionally retires destroyed
widget selection contexts and interrupted INCR storage; application-level
drag-and-drop workflows and other inherited dependency ownership remain separate
qualification gates.

## XPM parsing and file errors

The embedded XPM reader treats the terminating NUL of an in-memory input as an
end boundary, returns invalid input on an unterminated comment, and checks
unsigned numeric fields before decimal multiplication. It never searches past
the supplied string for a missing delimiter. Valid C, natural and Lisp syntax,
comments, multiple characters per pixel and image pixels retain their formats.
The array APIs still require the caller to supply the strings described by the
header; these historical APIs do not carry an array element count.

`XmeXpmWriteFileFromBuffer` supports empty buffers and reports write or final
flush/close failure. File-to-buffer reads use `size_t` for `fread`'s result and
close the owning stream on rejected sizes. These are storage and parsing
corrections, not automatic image scaling or a new rendering backend.

## Automated verification

Run `python3 tests/automated/run-storage-regressions.py PREFIX FRESH_RESULTS`
on Linux. The runner compiles against one absolute installed prefix, records
source/library fingerprints and commands, checks dynamic library resolution,
and gives each scenario an isolated Xvfb server and explicit temporary paths.
Use `--sanitize` with an instrumented toolkit for library coverage; the flag
alone only instruments the consumers. `--only` selects a diagnostic scenario.

The transfer fixture checks format 8/16/32 copies, append/retrieve, multi-request
properties, high-bit wire values, aliasing merges, rejected excessive counts,
and repeated named/primary/secondary/destination ownership destruction. The XPM
fixture places each truncated buffer immediately before a protected page,
checks unterminated file comments, numeric overflow, valid comments/syntax,
rendered pixel identities, empty files and `/dev/full` flush errors. These
fixtures do not establish every clipboard manager, compressed-image utility,
physical display or application workflow.
