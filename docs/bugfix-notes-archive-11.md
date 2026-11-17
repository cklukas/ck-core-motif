---
nav_exclude: true
---
# Bugfix Notes Archive 11

## `mrm-file-open-input.md`

# Mrm file-open input validation

The low-level Mrm file opener now rejects null filename, output-name, and file
handle pointers before dereferencing them. It also keeps the filename length in
`size_t` through the measured copy path, then converts the already-validated
allocation size explicitly to Xt's `Cardinal` contract. This avoids implicit
truncation on long filesystem names while preserving normal read and write
behavior.

The source-level regression is `tests/automated/check-mrm-file-open-input.py`.

## `mrm-file-seek-bounds.md`

# Mrm file seek checks

Mrm block reads and writes now check the `lseek` result before issuing the
system I/O call. A failed seek returns `MrmFAILURE` without reading or writing
at an unintended file position; valid record-number I/O is unchanged.

The regression extends `tests/automated/check-mrm-file-io-input.py`.

## `mrm-filename-suffix.md`

# Mrm hierarchy filename suffix validation

The hierarchy loader checks whether a UID filename already ends in `.uid`
before asking Xt to resolve it with a suffix. That check now rejects a null
filename and handles names shorter than four characters without indexing before
the string. Normal suffix resolution and fallback lookup remain unchanged.

The source-level regression is `tests/automated/check-mrm-filename-suffix.py`.

## `mrm-find-indexed-output.md`

# Mrm indexed-resource enumeration output validation

`UrmIdbFindIndexedResource` appends matching index strings to a caller-owned
pointer list. It now rejects a null list before validating the file or taking the
empty-database fast path, preventing a malformed call from reaching the index
walker with no output destination.

## `mrm-fixup-fontlist-validation.md`

# Mrm font-list fixup validation

`Urm__CW_FixupValue` now rejects missing font-list buffers and counts above the
format limit for both legacy and current font-list layouts before copying or
swapping items. Valid font lists retain their existing compatibility path.

The source-level regression is
`tests/automated/check-mrm-fixup-fontlist-validation.py`.

## `mrm-fixup-widechar-null.md`

# Mrm wide-character fixup null entries

`Urm__CW_FixupValue` now rejects a null serialized wide-character entry before
reading its count or converting its bytes. Valid wide-character values retain
the existing allocation and multibyte conversion behavior.

The source-level regression is
`tests/automated/check-mrm-fixup-widechar-null.py`.

## `mrm-fontlist-allocation.md`

# Mrm legacy font-list allocation

Legacy font-list conversion now checks the variable-size `XtMalloc` result
before fixup and replacement of the resource-context buffer. Allocation
failure returns `MrmFAILURE` without losing the original buffer; valid font
lists retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-fontlist-allocation.py`.

## `mrm-fontlist-bounds.md`

# Mrm legacy font-list bounds

Legacy Mrm 1.1 font-list conversion checks the item count before evaluating
`count - 1` and allocating the compatibility record. Zero, negative, or
unrepresentable counts—and records wider than Xt's `Cardinal` allocation
width—fail without underflow or truncation; valid font lists retain their
existing conversion behavior.

The source-level regression is `tests/automated/check-mrm-fontlist-bounds.py`.

## `mrm-fontlist-conversion-cleanup.md`

# MRM font-list conversion ownership

MRM now appends each converted font-set entry to the already constructed
`XmFontList`, rather than starting a new list for every font-set record.  A
UID font list containing multiple font sets therefore preserves all entries in
their original order.

The conversion releases each temporary `XmFontListEntry` after append, checks
entry/list creation, and frees a partially built list plus the display/font
lookup key on later conversion failures.  Successful public MRM and Xm font
list behavior is unchanged.

## `mrm-fontset-conversion-output.md`

# MRM standalone font-set conversion

When a UID resource has representation type `MrmRtypeFontSet` and the caller
does not request an `XmFontList`, MRM now returns the created or cached
`XFontSet` handle.  Previously this path returned the unrelated `XFontStruct`
local variable, which could give callers an invalid value.  Font resources and
font-list conversion keep their existing public behavior.

## `mrm-get-entry-postconditions.md`

# Mrm data-entry buffer postconditions

`Idb__DB_GetDataEntry` reads the buffer returned by the Mrm buffer manager for
both the initial record and each overflow-chain segment. It now verifies that
the buffer and its record storage are present after every acquisition before
forming record pointers, returning `MrmFAILURE` on an invalid postcondition.

## `mrm-guarantee-space.md`

# Mrm record-growth bounds

`UrmCWR__GuaranteeSpace` now keeps the aligned delta in `size_t` until it has
been checked against `MrmMaxResourceSize` and the current record size. This
prevents full-word rounding from wrapping a 16-bit delta to zero and rejects
record growth that would exceed the format limit.

The source-level regression is `tests/automated/check-mrm-guarantee-space.py`.

## `mrm-header-byte-swap-order.md`

# Mrm header byte-swap validation order

Header data-entry fields are stored in the file byte order. The header readers
now swap a foreign-endian entry header before validating its `validation` and
`entry_size` fields. This keeps valid byte-swapped files readable and ensures
capacity checks operate on the native size value.

## `mrm-header-data-bounds.md`

# Mrm header data-entry bounds

Header data-entry reads and writes now validate file/context/output handles,
header record type, item offsets, free-space metadata, and entry sizes before
accessing or extending the fixed header record.  Simple-entry sizes use
`MrmCount` arithmetic, avoiding truncation on large or malformed inputs.

The source-level regression is
`tests/automated/check-mrm-header-data-bounds.py`.

## `mrm-header-error-bounds.md`

# Mrm header diagnostic bounds

UID header-version mismatch and parse diagnostics use a fixed 300-byte buffer.
Those three formatting sites now use `snprintf`, preserving normal messages
while safely truncating malformed or unusually long on-disk version text.

The source-level regression is
`tests/automated/check-mrm-header-error-bounds.py`.

## `mrm-header-filter-lifetime.md`

# Mrm header filter buffer lifetime

`Idb__HDR_MatchFilter` now keeps the header buffer live while validating and
reading the data-entry header.  It computes the filter result first and only
then decommits the buffer, preventing a use-after-decommit while also checking
record and entry bounds.

The source-level regression is
`tests/automated/check-mrm-header-filter-lifetime.py`.

## `mrm-header-inputs.md`

# Mrm header helper inputs

Header loading, updating, RID access, and initialization now reject missing
file/output handles and incomplete header-record allocations before accessing
record fields.  RID retrieval clears a valid caller output handle before it
checks the file input, so every rejected request leaves neutral output state.
Valid header operations retain their existing behavior.

The source-level regression is `tests/automated/check-mrm-header-inputs.py`.

## `mrm-header-string-bounds.md`

# Mrm UID header string bounds

UID header fields have fixed-width on-disk storage. Header loading now requires
each field to contain a NUL terminator within its recorded width before copying
it into the in-memory `IDBFile`; malformed fields return `MrmNOT_VALID` instead
of allowing `strcpy` to read into adjacent header data or overflow a destination.
Valid headers retain their existing strings and version checks.

The source-level regression is
`tests/automated/check-mrm-header-string-bounds.py`.

## `mrm-header-version-allocation.md`

# Mrm header version parsing

`Idb__HDR_GetHeader` now parses both the on-disk and compiled-in database
version strings with a range-checked `strtol` helper. Malformed, negative, or
out-of-range major/minor values are rejected without invoking undefined
`sscanf` integer conversion behavior, and the compiled-in string no longer
needs a temporary allocation.

The source-level regression is
`tests/automated/check-mrm-header-version-allocation.py`.

## `mrm-header-write-input.md`

# Mrm header-writer input validation

`Idb__HDR_InitHeader` now rejects a null file handle or null creator/module
metadata before initializing the header buffer or passing those strings to
`strncat`. Valid header creation and its fixed-width truncation behavior remain
unchanged; malformed caller input returns `MrmFAILURE`.

The source-level regression is
`tests/automated/check-mrm-header-write-input.py`.

## `mrm-hget-output-pointer.md`

# Mrm hierarchy resource output validation

`UrmHGetIndexedResource` writes the file that supplied a resource through
`file_id_return`. It now rejects a null output pointer before traversing the
hierarchy, matching the lower-level file-open APIs and preserving valid lookup
behavior.

The output is cleared before the lookup starts and is assigned only after a
resource is found.  Failed or exhausted searches therefore cannot leave the
last attempted file exposed as a successful match.

## `mrm-hierarchy-bounds.md`

# Mrm hierarchy file-list bounds

Mrm hierarchy opening now rejects negative file counts and checks the file-list
byte multiplication against `SIZE_MAX` before allocating the primary and group
file vectors. The byte-size variable uses `size_t`, avoiding truncation through
the legacy `MrmCount` type while preserving valid hierarchy behavior.

The source-level regression is `tests/automated/check-mrm-hierarchy-counts.py`.

## `mrm-hierarchy-cleanup.md`

# Mrm hierarchy construction cleanup

`Urm__OpenHierarchy` now initializes every owned list before allocation and
runs `Urm__CloseHierarchy` when a file-list, group-list, or resource-context
allocation fails.  This releases already opened files and partial lists while
retaining the existing success and compatibility behavior.

The source-level regression is
`tests/automated/check-mrm-hierarchy-cleanup.py`.

## `mrm-hierarchy-name-inputs.md`

# Mrm hierarchy name API input checks

The hierarchy name-registration and lookup helpers validate the hierarchy
handle before dereferencing it. Registration also rejects null names in a
non-empty batch. Lookup rejects a null output pointer and clears a supplied
output before invalid-hierarchy, invalid-name, local-not-found, or global
fallback failure can return, so callers cannot retain a stale registered value.
Valid registrations and lookups retain their existing hash-table and
global-fallback behavior.

## `mrm-hierarchy-name-registration.md`

# Mrm hierarchy name registration

Hierarchy-local name registration now validates non-empty name/value arrays,
checks allocation of the hash table, and stops cleanly when a hash entry cannot
be created. Valid registrations retain their existing name-to-value mappings.

The source-level regression is
`tests/automated/check-mrm-hierarchy-name-registration.py`.

## `mrm-hierarchy-open-input.md`

# Mrm hierarchy-open input validation

Hierarchy opening rejects a missing output pointer, a missing filename list
when files are requested, and a missing in-memory UID buffer before opening
displays or allocating hierarchy state. After the output pointer is validated,
it is cleared before any failure-prone validation or allocation so a failed
open cannot leave a stale hierarchy handle. Valid file-backed and in-memory
opens retain their existing behavior.

The source-level regression is
`tests/automated/check-mrm-hierarchy-open-input.py`.

## `mrm-icon-bounds.md`

# Mrm icon allocation bounds

UID icon mapping validates dimensions, source strides, destination byte counts,
Xt's `Cardinal` allocation width, and descriptor raster sizes before allocating
image storage. Invalid or
unrepresentable icons return the existing Mrm failure result; ordinary icon
loading and XImage creation are unchanged.

The source-level regression is `tests/automated/check-mrm-icon-bounds.py`.

## Pixel values and the pixel size

`Urm__MapIconAllocate` replaces each pixel of an icon with the pixel value of
its colour table entry. The pixel value in the icon is data from the UID file
and can be as large as 255 for an 8 bit icon, whatever the number of entries in
the table. A value at or beyond the entry count of the table is a malformed
icon: the function frees its image storage and fails with `MrmFAILURE`, as it
does for the other malformed icons, instead of reading behind the table.

The function also reads the predefined background and foreground entries of the
table after the pixels are mapped. A missing table, or one that holds fewer than
the two predefined entries (`URMColorTableUserMin`), is rejected before any
storage is allocated.

The pixel size of an icon is one of four codes (`URMPixelSize1Bit` to
`URMPixelSize8Bit`) that select the number of bits per pixel. Any other value
used to leave that number at zero, so the loop over the bits of a byte never
advanced and the call never returned. An unknown pixel size is now rejected
before any storage is allocated. Valid icons are mapped as before.

### Test

```text
python3 tests/automated/run-mrm-icon-bounds.py RESULTS
python3 tests/automated/run-mrm-icon-bounds.py RESULTS2 --source-revision HEAD
```

`check-mrm-icon-bounds.c` calls the production `Urm__MapIconAllocate` under
AddressSanitizer and UndefinedBehaviorSanitizer with icons and colour tables
that have exactly the size of their contents (the display is the stand-in of
`tests/fuzz`). Pixel values beyond the table, an unknown pixel size (bounded by
an alarm) and a missing or too small table fail cleanly; valid 1, 4 and 8 bit
icons are mapped. The crash input that found the defect is kept as a seed in
`tests/fuzz/corpus/mrm-uid`.

## `mrm-icon-colortable-count.md`

# Mrm icon color-table count validation

A color table always reserves entries zero and one for background and
foreground pixels. `Urm__RealizeColorTable` now rejects a table whose count is
below `URMColorTableUserMin` before writing those reserved entries. This
prevents malformed UID data with a zero or one entry from causing an
out-of-bounds write while preserving all valid color tables.

The source-level regression is
`tests/automated/check-mrm-icon-colortable-count.py`.

## `mrm-icon-colortable-validation.md`

# Mrm icon color-table validation

Icon conversion requires a serialized color table. `UrmCreatePixmap` now
rejects an icon whose color-table pointer is missing before color realization or
mapping, returning `MrmNOT_VALID` while preserving a cleared output. The
shared `Urm__RealizeColorTable` helper also rejects a null table so direct
internal callers cannot reach its first table-entry write with invalid data.

Valid icon conversion and color allocation are unchanged. The source-level
regression is `tests/automated/check-mrm-icon-colortable-validation.py`.

## `mrm-icon-converter-inputs.md`

# Mrm icon converter input validation

`UrmCreatePixmap` and `UrmCreateBitmap` consume icon records loaded from UID
resources and return an X pixmap through a caller-owned pointer. Both helpers
now reject a missing icon or result pointer before reading the record, and
clear a valid result pointer before conversion begins. They also reject a
nonpositive width or height and a missing in-memory pixel buffer before the
mapping routines perform stride arithmetic or pass dimensions to Xlib.

The previous code dereferenced `icon` immediately and passed `pixmap` to lower
mapping routines without checking it. A malformed resource or invalid internal
call could therefore crash instead of returning `MrmFAILURE`. Successful
conversion and format validation are unchanged; malformed records continue to
use the established `MrmNOT_VALID` result. The source-level regression is
`tests/automated/check-mrm-icon-converter-inputs.py`.

## `mrm-icon-error-bounds.md`

# Mrm icon diagnostic bounds

Icon loading and color realization report filenames, color names, and decoded
color values through a fixed 300-byte diagnostic buffer. Those messages now
use `snprintf`, preserving normal diagnostics while safely truncating unusually
long input instead of overwriting stack storage.

The source-level regression is `tests/automated/check-mrm-icon-error-bounds.py`.

## `mrm-icon-gc-cleanup.md`

# Mrm icon graphics-context cleanup

All four icon-mapping implementations create an `XImage` and a server pixmap
before acquiring the graphics context used for `XPutImage`. If graphics context
creation fails, they now release both partial resources and reset the output
pixmap to zero before reporting failure. Successful icon mapping is unchanged.

The focused regression is `tests/automated/check-mrm-icon-gc-cleanup.py`.

## `mrm-icon-loader-inputs.md`

# Mrm icon loader input validation

`Urm__CW_LoadIconImage` fixes up an icon record, relocates its color table,
and appends referenced resource contexts. It now rejects a missing icon,
serialized buffer, or context list before reading the validation field or
performing pointer arithmetic.

This turns malformed or incomplete literal contexts into `MrmFAILURE` instead
of allowing an immediate null dereference. Valid loader calls retain the same
relocation and resource-loading behavior. The source-level regression is
`tests/automated/check-mrm-icon-loader-inputs.py`.

## `mrm-icon-pixargs-bounds.md`

# Deferred icon argument bounds

`Urm__CW_CreateArglist` defers icon-image arguments until a widget has been
created so it can supply the display and colour parameters needed to build a
pixmap. The deferred storage is a fixed compatibility buffer with ten entries.
The ordinary icon-image path now checks that capacity before loading or storing
another icon, matching the existing check for literal-backed icons and bitmap
files. Excess malformed arguments are reported and skipped instead of writing
past the local array.

## `mrm-icon-record-bounds.md`

# Mrm icon record bounds

Mrm icon argument values now validate the serialized icon size before passing it
to record storage. Null or invalid images and records larger than the format's
16-bit `MrmSize` limit are rejected instead of being truncated during
`GuaranteeSpace`; valid icons retain their existing copy behavior.

The source-level regression is `tests/automated/check-mrm-icon-record-bounds.py`.

## `mrm-idb-block-io.md`

# Mrm IDB block I/O

IDB block reads and writes now retry interrupted system calls and continue
short transfers until the complete fixed-size record is processed. A zero or
other unrecoverable result still returns `MrmFAILURE`.

The source-level regression is `tests/automated/check-mrm-idb-block-io.py`.

## `mrm-idb-buffer-cleanup.md`

# Mrm in-memory IDB cleanup

When an in-memory UID buffer fails header validation, `UrmIdbOpenBuffer` now
releases the descriptor it allocated before returning the validation error.
Successful opens retain their existing ownership and lifetime behavior.

The source-level regression is `tests/automated/check-mrm-idb-buffer-cleanup.py`.

## `mrm-idb-close-delete-mode.md`

# Mrm IDB close-error delete mode

The IDB close-error path now deletes the underlying file only for write-access
handles. Read-only files are closed and released without being unlinked when a
flush or header operation fails.

The source-level regression is
`tests/automated/check-mrm-idb-close-delete-mode.py`.

## `mrm-idb-close-keep.md`

# Mrm IDB close keep/discard behavior

`UrmIdbCloseFile` now honors its `keep_new_file` argument. Write-access files
update their header and remain on disk only when the flag is true; successful
discard closes delete the working file. Read-only closes continue to preserve
the source file.

The source-level regression is `tests/automated/check-mrm-idb-close-keep.py`.

## `mrm-idb-close-status.md`

# Mrm IDB close status and cleanup

Low-level IDB close now frees its state when the operating-system close fails,
and reports unlink failures when deleting a file. The public `UrmIdbCloseFile`
entry point now returns the low-level close result instead of always reporting
success.

The source-level regression is `tests/automated/check-mrm-idb-close-status.py`.

## `mrm-idb-decommit-cleanup.md`

# Mrm IDB decommit cleanup

`Idb__BM_DecommitAll` now attempts every buffer belonging to a file, records
the first write error, and detaches each buffer before returning. Failed
buffers are marked inactive and unmodified so a later close cannot retain a
dangling file pointer after the file handle is released.

The source-level regression is `tests/automated/check-mrm-idb-decommit-cleanup.py`.

## `mrm-idb-empty-decommit.md`

# Mrm empty IDB decommit

Closing an IDB file before any record buffer has been allocated now treats the
empty buffer pool as a successful no-op. This prevents a false decommit failure
from entering the close-error path and deleting or otherwise mishandling a
valid file.

The source-level regression is `tests/automated/check-mrm-idb-empty-decommit.py`.

## `mrm-idb-indexed-inputs.md`

# Mrm indexed-resource input checks

`UrmIdbGetIndexedResource` requires an index string and a resource context for
the returned data block. It now rejects either null input before invoking the
index and data-layer helpers, preventing null dereferences while preserving the
existing behavior for valid lookups.

## `mrm-idb-open-cleanup.md`

# Mrm low-level IDB open cleanup

Low-level IDB opens now close the operating-system file descriptor when
allocation of the descriptor or its owned filename fails. This prevents file
descriptor leaks while preserving the existing error results.

The source-level regression is `tests/automated/check-mrm-idb-open-cleanup.py`.

## `mrm-idb-open-output-pointers.md`

# Mrm IDB open output handles

The IDB file and in-memory-buffer open helpers now reject missing output
handles before opening or allocating state. In-memory opens also reject a
missing UID buffer, and the write-open path closes its low-level file cleanly
when descriptor allocation fails.

The source-level regression is
`tests/automated/check-mrm-idb-open-output-pointers.py`.

The low-level file opener follows the same contract.  Once its output pointer
has been validated, it clears that output before inspecting or opening the
requested pathname.  A failed open therefore cannot leave a caller holding a
stale low-level file handle.

## `mrm-idb-record-bounds.md`

# Mrm in-memory IDB record bounds

In-memory IDB record access now rejects nonpositive record numbers and records
beyond the header's logical last-record value before calculating a UID-buffer
offset. The header record remains available during initial open validation.

The source-level regression is `tests/automated/check-mrm-idb-record-bounds.py`.

## `mrm-idb-write-conflict-cleanup.md`

# Mrm IDB write-open conflict cleanup

When a write-open finds an existing file but cannot proceed because the OS
extension is missing, clobbering is disabled, or the extension version is
invalid, the already-open descriptor is now closed before returning. This
prevents descriptor leaks while preserving each existing status code.

The source-level regression is
`tests/automated/check-mrm-idb-write-conflict-cleanup.py`.

## `mrm-index-buffer-validation.md`

# Mrm index buffer validation

Index insertion and child-repair helpers now reject missing record storage and
invalid parent identifiers before dereferencing buffers.  The local record
validation predicates also handle null buffers, allowing split and repair
paths to report a bad record instead of crashing.

The source-level regression is
`tests/automated/check-mrm-index-buffer-validation.py`.

## `mrm-index-collapse-bounds.md`

# Mrm index collapse bounds

Leaf and node index-collapse helpers now reject missing records, reversed or
out-of-range entry spans, impossible index counts, and malformed string
offsets before copying data.  They also bound temporary-heap growth, preserving
valid index splits while preventing malformed records from causing underflow,
out-of-record reads, or heap overwrites.

The source-level regression is
`tests/automated/check-mrm-index-collapse-bounds.py`.

## `mrm-index-collapse-copy-bounds.md`

# Mrm index collapse copy bounds

The leaf and node index collapse helpers now compute a bounded source length
and rounded storage size before copying a string into their temporary heap.
Oversized or corrupt aggregate strings are rejected before the copy can write
past the allocation, while valid serialized indexes retain their existing
layout.

The source-level regression is
`tests/automated/check-mrm-index-collapse-copy-bounds.py`.

## `mrm-index-collapse-status.md`

# Mrm index collapse status

Index collapse helpers now return `Cardinal` status values.  Allocation and
record-validation failures are propagated by both leaf and node split paths,
which prevents a split from publishing partially rebuilt records.  Successful
collapses retain the existing serialized index layout and return
`MrmSUCCESS`.

The source-level regression is
`tests/automated/check-mrm-index-collapse-status.py`.

## `mrm-index-entry-bounds.md`

# Mrm index insertion bounds

Leaf and node insertion now validate record counts, heap offsets, free-space
metadata, and insertion positions before moving index vectors or subtracting
string storage.  Unexpected search results are propagated safely, preserving
normal B-tree insertion while rejecting malformed in-memory records.

The source-level regression is
`tests/automated/check-mrm-index-entry-bounds.py`.

## `mrm-index-entry-input.md`

# Mrm index-entry input validation

The high-level Mrm index entry routine and both root-record initializers now
validate their file, output-buffer, and index pointers before touching index
state. Valid index creation remains unchanged; malformed input returns
`MrmFAILURE` before allocating or dereferencing a record.

The source-level regression is
`tests/automated/check-mrm-index-entry-input.py`.

## `mrm-index-inputs.md`

# Mrm index traversal inputs

Mrm index search and B-tree traversal helpers now reject missing file handles,
search strings, record buffers, and output pointers before dereferencing them.
This preserves valid traversal while turning malformed internal calls into a
clean `MrmFAILURE`.

The source-level regression is `tests/automated/check-mrm-index-inputs.py`.

## `mrm-index-list-failure.md`

# Mrm index list allocation failures

`Idb__INX_FindResources` builds its result through `UrmPlistAppendString`.
Both leaf and node traversal paths now propagate an append failure instead of
returning `MrmSUCCESS` with an incomplete resource list.

## `mrm-index-mutation-inputs.md`

# Mrm index mutation inputs

Index split, node-space, and parent-link helpers now reject missing record
buffers, missing file handles, and invalid parent or child record numbers
before accessing record headers.

The source-level regression is
`tests/automated/check-mrm-index-mutation-inputs.py`.

## `mrm-index-parent-errors.md`

# Mrm index parent-update errors

`Idb__INX_EnterNodeIndex` updates the parent record number of both child
records after inserting a node entry.  The left-child update is now checked
before the result variable is reused for the right-child update.  A failure
updating either child is therefore returned to the caller instead of being
silently hidden by a later success.

The source-level regression is
`tests/automated/check-mrm-index-parent-errors.py`.

## `mrm-index-pivot-scan.md`

# Mrm index pivot scan bounds

Leaf and node index splits now bound pivot-name scans by both the maximum index
name length and the bytes remaining in the serialized record.  A corrupt offset
near the record boundary cannot make `strnlen` read beyond the record before
the existing malformed-pivot rejection runs.

The source-level regression is `tests/automated/check-mrm-index-pivot-scan.py`.

## `mrm-index-preflight.md`

# Mrm index insertion preflight

`Idb__INX_EnterNodeIndex` now checks the neighboring child links before
shifting entries, writing the new index, or changing free-space metadata.  A
malformed B-tree therefore fails without leaving a partially inserted node.
Successful insertions retain the existing ordering and parent-repair behavior.

The source-level regression is `tests/automated/check-mrm-index-preflight.py`.

## `mrm-index-read-bounds.md`

# Mrm read-side index bounds

Read-side index search and resource enumeration now use `MrmCount` for entry
indices, handle empty records without unsigned underflow, validate string
offsets and node metadata, and propagate buffer reload failures.  Malformed
child links and unterminated index strings now return a bad-record result
instead of crashing or walking arbitrary memory.

The source-level regression is
`tests/automated/check-mrm-index-read-bounds.py`.

## `mrm-index-record-copies.md`

# Mrm index-record copy inputs

The leaf and node index-record copy helpers now return immediately when either
record pointer is missing.  This keeps failed record allocations from turning
index maintenance into a null-pointer crash while preserving the existing
fixed-size copy behavior for valid records.

The source-level regression is
`tests/automated/check-mrm-index-record-copies.py`.

## `mrm-index-return-inputs.md`

# Mrm index item lookup inputs

`Idb__INX_ReturnItem` now rejects missing file handles, index strings, and
output data handles before traversing the index record.

The source-level regression is `tests/automated/check-mrm-index-return-inputs.py`.

## `mrm-index-split-bounds.md`

# Mrm index split bounds

Index splits now reject impossible record counts and malformed pivot strings
before hoisting an entry.  Failed left-record allocation is checked before the
new buffer is used.  Empty right-hand partitions produced by a valid two-entry
split are handled explicitly, preserving normal B-tree behavior while keeping
malformed ranges inert.

The source-level regression is
`tests/automated/check-mrm-index-split-bounds.py`.

## `mrm-index-string-bounds.md`

# Mrm index string bounds

Index entries store offsets into a fixed record string area. Search and resource
enumeration now use a shared terminator check that limits `strnlen` to both the
maximum index-name length and the bytes remaining in the record. Corrupt
near-end offsets therefore fail as bad records without reading beyond the
record buffer.

## `mrm-index-writer-input.md`

# Mrm index-writer input validation

The low-level leaf and node index insertion routines now reject null file,
record-buffer, or index pointers before reading the index length. Valid index
insertion and record-splitting behavior remain unchanged; malformed writer
input returns `MrmFAILURE` instead of faulting in `strlen`.

The source-level regression is
`tests/automated/check-mrm-index-writer-input.py`.

## `mrm-interface-module-null.md`

# Mrm interface-module null guards

Interface-module readers now reject a valid resource context whose data buffer
is null before reading `RGMModuleDesc.validation`. Shared module-binding and
direct module-get paths return `MrmBAD_IF_MODULE` for empty data instead of
dereferencing a null pointer.

## `mrm-legacy-fontlist-fixup.md`

# Mrm legacy font-list fixup failure

Legacy font-list replacement now frees its temporary replacement and returns the
fixup error before replacing the resource-context buffer. The same ownership
rule applies to widget resource updates and callback literal conversion.

The focused regression is `tests/automated/check-mrm-fontlist-allocation.py`.

## `mrm-literal-bounds.md`

# UID files: units literals and data entry offsets

## Units literals must be as large as their record

A UID literal of type `HorizontalInteger`, `VerticalInteger`, `HorizontalFloat` or
`VerticalFloat` is a record (`RGMUnitsInteger`, `RGMUnitsFloat`) that
`Urm__CW_ConvertValue` reads as a whole. The UIL compiler writes it with the
size of the record, but the size in a file is data like any other: a shorter
literal made the converter read its value and units beyond the buffer that holds
the literal.

The size of the literal is checked against the size of the record before it is
fixed up or converted, in `Urm__FetchLiteral` (`MrmFetchLiteral`) and in
`Urm__CW_ReadLiteral` (`MrmFetchSetValues`, the widget argument lists and the
callback fixup), and a value with units that is stored inside a widget record is
checked against the end of that record. A literal that is too short is a corrupt
file: the fetch fails (`MrmNOT_VALID` inside the library, `MrmFAILURE` from
`MrmFetchLiteral`) for that literal, like the other malformed literals, and the
argument is not applied.

## Data entry offsets must be aligned

The data entries of a data record and of the header record are read in place
through a structure with 4 byte alignment. The writer places every entry at a
full-word offset of the record data, so the offset in a handle is always a
multiple of four. An offset from a file that is not aligned is now rejected
as a corrupt file by `Idb__DB_GetDataEntry`, `Idb__HDR_GetDataEntry`, and the
two filter matchers, and the writers refuse to append behind a damaged free
pointer of that kind. Before, the read was an access to a misaligned structure,
which is undefined behaviour in C even where the hardware tolerates it.

## Test

```text
python3 tests/automated/run-mrm-literal-bounds.py RESULTS
python3 tests/automated/run-mrm-literal-bounds.py RESULTS2 --source-revision HEAD
```

`check-mrm-literal-bounds.c` writes UID files with the resource manager's own
writer and reads them back with the production `lib/Mrm` sources under
AddressSanitizer and UndefinedBehaviorSanitizer (the display and the widget
library are the stand-ins of `tests/fuzz`). Valid float and integer literals,
and aligned entries of data and header records, still load. The crash inputs that
found the defects are kept as seeds in `tests/fuzz/corpus/mrm-uid`.

## `mrm-literal-context-init.md`

# Mrm literal context initialization

Mrm literal loading now checks the result of `UrmGetResourceContext` before
reading or storing data through the returned context.  Allocation failure is
reported to the caller, while successful literal loading keeps the existing
resource and callback behavior.

The source-level regression is
`tests/automated/check-mrm-literal-context-init.py`.

## `mrm-literal-context-retain.md`

# Mrm literal context retention

After reading a literal, Mrm records the resource context in the caller-owned
context list so nested references can be released together.  If that list
cannot accept the pointer, the loader now releases the context immediately and
returns the append failure instead of leaking it or continuing with incomplete
ownership tracking.

The source-level regression is
`tests/automated/check-mrm-literal-context-retain.py`.

## `mrm-literal-convenience-outputs.md`

# Mrm literal convenience output validation

The public `MrmFetchIconLiteral`, `MrmFetchBitmapLiteral`, and
`MrmFetchColorLiteral` helpers now validate their caller-owned output pointers
before entering the locked fetch path. The bitmap helper validates its pixmap,
width, and height outputs together; the icon and color helpers validate their
single output. Valid outputs are cleared before the fetch begins, so a failed
lookup cannot leave stale result values visible to the caller.

Previously, a null bitmap width or height was dereferenced directly, while null
icon and color outputs could reach lower-level conversion helpers and crash.
Missing outputs now return `MrmFAILURE` without acquiring locks or allocating a
resource context. Existing successful calls retain their lookup and conversion
behavior. `MrmFetchBitmapLiteral` additionally leaves the dimensions at zero
when the depth-one pixmap conversion fails; nonzero dimensions now accompany a
successfully returned pixmap only.

The source-level regression is
`tests/automated/check-mrm-literal-convenience-outputs.py`.

## `mrm-literal-error-bounds.md`

# Mrm literal diagnostic bounds

Mrm literal-fetch and color-literal error paths format into fixed 300-byte
buffers. Both paths now use `snprintf`, preserving normal diagnostics while
safely truncating long literal indexes and malformed input text.

The source-level regression is
`tests/automated/check-mrm-literal-error-bounds.py`.

## `mrm-match-filter-lifetime.md`

# Mrm filter buffer lifetime

`Idb__DB_MatchFilter` obtains a data record and inspects its entry header to
apply group and type filters. The buffer must remain active for those reads;
decommitting before inspection leaves the routine holding a buffer that the
cache may reuse. The routine now validates and filters the entry first, then
decommits on every validation or filter exit path.

## `mrm-memory-close.md`

# Mrm memory-backed file cleanup

UID files opened from a caller-owned memory buffer do not have a low-level file
handle. `UrmIdbCloseFile` now skips the filesystem close for those descriptors,
returns success, and still releases the descriptor and compression tables.
Hierarchy cleanup now closes every file descriptor, including memory-backed
ones, so in-memory hierarchies do not leak their per-file state.

## `mrm-module-size-bounds.md`

# Mrm module descriptor size bounds

Interface-module construction now keeps descriptor sizing in `size_t`, checks
the variable-count multiplication and full-word alignment, and rejects sizes
larger than the 16-bit Mrm resource limit before resizing the context. Valid
module counts retain their existing layout and initialization behavior.

The legacy `MrmCount` parameter is signed.  Negative values are rejected
before descriptor-size arithmetic, so an out-of-range caller-side conversion
cannot become a huge `size_t` allocation request.

The source-level regression is
`tests/automated/check-mrm-module-size-bounds.py`.

## `mrm-no-clobber-close.md`

# Mrm no-clobber close handling

The no-clobber write-open path now checks the close result for the existing
file descriptor before returning `MrmEXISTS`. A close failure returns
`MrmFAILURE`, avoiding a leaked descriptor and an inaccurate status.

## `mrm-open-name-input.md`

# Mrm file-open filename validation

`UrmIdbOpenFileRead` requires a filename for the low-level file opener. It now
rejects a null name together with the existing output-pointer check, while
leaving the optional resolved-filename output buffer unchanged for callers that
do not need it. It also rejects names whose terminator-inclusive allocation
cannot be represented by Xt's `Cardinal` size before copying the optional
resolved name or opening the file.

## `mrm-open-output-init.md`

# Mrm open output initialization

`UrmIdbOpenFileRead` and `UrmIdbOpenBuffer` now clear the caller's output handle
immediately after validating the output pointer. Failed opens therefore cannot
leave a stale descriptor that a caller might mistake for the newly opened file;
successful opens still replace the output with the new descriptor as before.

## `mrm-open-recovery.md`

# Mrm write-open recovery

When creating a write-mode database, Mrm first creates and closes the file,
then reopens it for read/write access. If that reopen fails, the recovery path
now unlinks the partial file before returning failure, avoiding a misleading
empty database artifact.

## `mrm-open-write-output.md`

# Mrm write-open output initialization

`UrmIdbOpenFileWrite` now clears `file_id_return` before opening the low-level
file or allocating the descriptor. A failed create, allocation, or header
initialization therefore cannot leave a stale handle in the caller’s output
slot. Successful write opens retain their existing behavior.

## `mrm-openhierarchy-os-input.md`

# Mrm hierarchy open input validation

`Urm__OpenHierarchy` accepts an optional pointer to platform-specific open
parameters. A non-null outer pointer must contain a valid parameter object; the
old path dereferenced a null object before it could report an error. The input
check now rejects that malformed combination while keeping the existing null
outer-pointer behavior for callers that do not use platform-specific options.

## `mrm-pixmap-defer-bounds.md`

# Mrm deferred pixmap bounds

`Urm__CW_CreateArglist` now checks the ten-entry deferred pixmap array before
saving icon-image or X bitmap arguments.  Excess deferred entries are rejected
without writing past the stack array; existing argument lists within the
historic capacity retain their behavior.

The source-level regression is
`tests/automated/check-mrm-pixmap-defer-bounds.py`.

## `mrm-pixmap-tag-failure.md`

# Deferred pixmap tag failures

When `Urm__CW_CreateArglist` finishes a deferred icon or bitmap argument, it
must decompress the resource tag before appending the argument. A failed tag
lookup previously left the name slot undefined and still increased the output
count. The finalization path now reports the lookup failure, releases the
pixmap it just created, and skips that argument, preserving the remaining
argument list and preventing a leaked server resource.

## `mrm-pointer-list-append-state.md`

# Mrm pointer-list append state

`UrmPlistAppendPointer` now rejects a null vector, non-positive slot count,
negative active count, or active count beyond the slot count before indexing
the list. This keeps partially initialized or inconsistent lists on a safe
failure path.

## `mrm-pointer-list-cleanup.md`

# Mrm pointer-list append cleanup

`UrmPlistAppendString` allocates a copy before growing the pointer list. If the
list resize or append fails, the newly allocated copy is now freed before the
failure is returned. Successful appends retain ownership in the list as
before.

## `mrm-pointer-list-content-state.md`

# Mrm pointer-list content cleanup state

`UrmPlistFreeContents` now rejects a missing vector or inconsistent active and
slot counts before walking the list. This protects cleanup of partially
initialized lists while preserving repeatable cleanup for valid lists.

## `mrm-pointer-list-free.md`

# Mrm pointer-list cleanup

`UrmPlistFree` and `UrmPlistFreeContents` now reject null list handles safely.
`UrmPlistFreeContents` also clears each freed slot and resets the active count,
so repeated cleanup cannot double-free stale entries and the list can be reused.

## `mrm-pointer-list-init-cleanup.md`

# Mrm pointer-list initialization cleanup

If the pointer-vector allocation fails, `UrmPlistInit` now frees the list
structure it already allocated and clears the output pointer before returning
failure. Successful initialization and ownership rules are unchanged.

## `mrm-pointer-list-input.md`

# Mrm pointer-list input validation

Mrm pointer-list initialization, resizing, append, and string-search helpers
now validate their list and string pointers before dereferencing them. Invalid
inputs return the existing `MrmFAILURE` or “not found” result; valid list
allocation, growth, ownership, and lookup behavior is unchanged.

The source-level regression is
`tests/automated/check-mrm-pointer-list-input.py`.

## `mrm-pointer-list-resize.md`

# Mrm pointer-list resize ownership

`UrmPlistResize` now stores the `XtRealloc` result in a temporary pointer and
updates the list only after allocation succeeds. A failed resize therefore
leaves the existing pointer vector and slot count intact for the caller to
handle.

## `mrm-pointer-list-search.md`

# Mrm pointer-list string search

`UrmPlistFindString` now skips null pointer slots before calling `strcmp`.
This keeps searches safe when callers append a null pointer or reuse a list
whose contents were cleared.

## `mrm-pointer-list-shrink.md`

# Mrm pointer-list resize limits

`UrmPlistResize` now rejects a requested slot count below the number of active
pointers. This prevents callers from truncating live entries and leaving the
list's ownership and indexing state inconsistent.

## `mrm-pointer-list-string-bounds.md`

# Mrm pointer-list string bounds

Pointer-list string appends now keep the source length in `size_t`, reject an
unrepresentable terminator-inclusive allocation for Xt's `Cardinal` width,
and copy the validated bytes directly. Valid appends retain their existing
ownership and list behavior; malformed sizes return the existing `MrmFAILURE`
path safely.

The source-level regression is
`tests/automated/check-mrm-pointer-list-string-bounds.py`.

## `mrm-pointerlist-bounds.md`

# Mrm pointer-list bounds

Mrm pointer lists now reject non-positive capacities and capacity products that
cannot be represented by either `size_t` or Xt's `Cardinal` allocation width
before allocating their pointer vectors.
Appending to a malformed or already-full list also refuses to double a slot
count past `INT_MAX`, avoiding signed overflow and wrapped allocator sizes.
Valid positive capacities and the existing append/resize behavior are unchanged.

The source-level regression is `tests/automated/check-mrm-pointerlist-bounds.py`.

## `mrm-put-entry-capacity.md`

# Mrm data-entry insertion capacity

Before adding a simple entry to an existing data record, Mrm now validates the
record's `free_ptr` and `free_count` against the fixed payload size and checks
that their combined range remains inside the record. The check is repeated
after allocating a replacement record, so malformed on-disk metadata cannot
make the writer form an out-of-bounds destination pointer.

## `mrm-put-entry-inputs.md`

# Mrm data-entry writer inputs and sizes

`Idb__DB_PutDataEntry` now rejects a missing file handle or output data handle
before calling the header or buffer writers. Its record-size arithmetic also
uses unsigned/wider locals for entry, remaining, and segment sizes, avoiding
signed 16-bit overflow while retaining the existing 16-bit on-disk fields and
public API.

## `mrm-reader-text-vector-validation.md`

# Mrm reader text-vector validation

`Urm__CW_FixupValue` now rejects null, invalid, or over-sized text vectors
before swapping entries or walking their offsets. Valid Char8 and compound
string vectors retain the existing fixup behavior, while malformed resource
records fail cleanly instead of causing an out-of-bounds traversal.

The source-level regression is
`tests/automated/check-mrm-reader-text-vector-validation.py`.

## `mrm-record-type.md`

# Mrm record-type validation

`Idb__BM_InitRecord` updates the per-file record-type counters after creating a
record. Those counters have a fixed `IDBrtMin` through `IDBrtMax` range. The
initializer now rejects values outside that range before acquiring or mutating
a buffer, preventing an invalid type from indexing the counter array and
leaving a partially initialized record behind.

## `mrm-register-counts.md`

# Mrm name-registration counts

The public Mrm name-registration APIs now reject negative or unrepresentable
entry counts before allocating parallel name and value vectors. A zero-entry
registration is treated as a successful no-op, and the internal global and
hierarchy registration paths reject negative counts as well. Valid registrations
retain their existing behavior.

The source-level regression is `tests/automated/check-mrm-register-counts.py`.

## `mrm-register-names-allocation.md`

# Mrm name-registration allocation handling

Global and hierarchy-scoped name registration now reject a nonzero count with a
null registration list and reject vector sizes that exceed either host
`size_t` or Xt's `Cardinal` allocation width. Both temporary name/value vector
allocations are checked.
Failure paths release any vector already allocated and unlock the Mrm process
state before returning.

The source-level regression is
`tests/automated/check-mrm-register-names-allocation.py`.

## `mrm-related-count-type.md`

# Related Mrm argument types

A serialized related code supplies the count resource for a text, string, or
integer vector. A malformed record could attach a related code to another
representation type; the old path then appended an argument whose value had
never been initialized. The reader now leaves the already validated primary
argument in place and skips that invalid related entry, preserving compatibility
for valid vector resources while avoiding undefined argument data.

## `mrm-related-tag-diagnostic.md`

# Mrm related-tag diagnostic handling

Related resource-tag lookup failures now report the related compressed code,
not the primary tag code, and stop the failed argument before later processing.
This makes diagnostics actionable and avoids retaining a partially initialized
related argument entry.

The source-level regression is
`tests/automated/check-mrm-related-tag-diagnostic.py`.

## `mrm-related-tag-lookup-failure.md`

# Mrm primary tag lookup failure handling

A failed compressed primary-tag lookup now skips the rest of that argument.
Previously the conversion could continue into foreground/background checks and
related-count handling using the prior argument’s name slot.  Valid compressed
and unknown tags keep their existing behavior.

The source-level regression is
`tests/automated/check-mrm-related-tag-lookup-failure.py`.

## `mrm-resolved-path-output.md`

# Mrm resolved path output

Hierarchy loading resolves UID paths through Xt and can produce names longer
than the old 300-byte local scratch buffer.  The resolved-name path now omits
that unused output buffer, and the low-level open helper treats the output name
as optional while preserving existing callers that provide storage.

The source-level regression is
`tests/automated/check-mrm-resolved-path-output.py`.

## `mrm-resource-bounds.md`

# Mrm resource-descriptor bounds

Mrm widget-record resource descriptors now check index-string lengths before
adding them to the descriptor size and storing that size in the 16-bit
`MrmSize` field. Oversized indexes return `MrmFAILURE`; valid resource indexes
retain their existing record layout.

The source-level regression is `tests/automated/check-mrm-resource-bounds.py`.

## `mrm-resource-id-output-init.md`

# Mrm resource-id output initialization

`UrmIdbGetResourceId` now initializes its output to zero before validating the
file or allocating the next resource ID. The local RID is also initialized,
so an allocation failure cannot copy an indeterminate value to the caller.
Successful calls preserve the historical returned ID.

## `mrm-resource-id-output.md`

# Mrm resource-id output validation

`UrmIdbGetResourceId` writes the next resource identifier through
`resource_id_return`. It now rejects a null output pointer before validating the
file and before the RID allocator runs, preventing a null write while retaining
valid identifier allocation behavior.

## `mrm-resource-index.md`

# Mrm indexed-resource keys

Mrm indexed-resource construction now rejects a null index with the existing
`MrmNULL_INDEX` result before calling `strlen`. Valid non-empty index strings
retain their existing descriptor behavior.

The source-level regression is `tests/automated/check-mrm-resource-index.py`.

## `mrm-resource-parent-rebind.md`

# Mrm resource parent rebinding

`Idb__INX_FindResources` now reloads and validates the parent node at the start
of every node-entry iteration.  Recursive child traversal can turn over the
buffer cache, so reading the next entry only from the rebound node avoids stale
record pointers while preserving traversal order and filtering behavior.

The source-level regression is
`tests/automated/check-mrm-resource-parent-rebind.py`.

## `mrm-resource-record-bounds.md`

# Mrm resource-descriptor bounds

Embedded `RGMResourceDesc` values now validate their declared size before field
access.  RID descriptors must contain the fixed record, while indexed
references must contain a NUL-terminated key within the declared bytes.  The
same check protects ordinary argument conversion and callback resource values;
external literal contexts continue through their existing reader path.

The source-level regression is
`tests/automated/check-mrm-resource-record-bounds.py`.

## `mrm-resource-reload-bounds.md`

# Mrm resource reload bounds

`Idb__INX_FindResources` now revalidates the current node entry’s string after
buffer turnover reloads the node record.  The traversal therefore checks the
actual rebound record before appending its name, preserving safe behavior when
serialized index data is malformed.

The source-level regression is
`tests/automated/check-mrm-resource-reload-bounds.py`.

## `mrm-resource-tag-lookup-failure.md`

# Mrm resource-tag lookup failure handling

When compressed resource-tag lookup fails, `Urm__CW_CreateArglist` now skips
the malformed argument immediately after reporting the lookup error.  It no
longer falls through to `strcmp` with an unset resource-name pointer.  Valid
compressed and unknown tags retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-resource-tag-lookup-failure.py`.

## `mrm-rid-context.md`

# Mrm RID-resource context validation

`UrmIdbGetRIDResource` fills a caller-provided resource context and reads its
filters after the data lookup. It now rejects a null context before entering the
file and RID lookup path, preventing a null dereference while preserving valid
RID-resource behavior.

## `mrm-rid-handle-bounds.md`

# Mrm RID-map handle bounds

RID-map entries store pointers to data or header records. `Idb__RID_EnterItem`
and `Idb__RID_ReturnItem` now reject non-positive record numbers and apply the
correct header/data-record offset bound before storing or returning a handle.
Empty RID entries retain their existing null-result behavior.

## `mrm-rid-inputs.md`

# Mrm resource-ID helper inputs

Resource-ID map helpers now reject missing file handles, output handles, and
incomplete map-record allocations before dereferencing them.  Valid RID
lookups and allocation behavior remain unchanged.

The source-level regression is `tests/automated/check-mrm-rid-inputs.py`.

## `mrm-root-allocation.md`

# Mrm root-record allocation checks

Root leaf and node initialization now verifies that a successful buffer
allocation includes both a buffer object and record storage before initializing
the header.  Incomplete allocator results now return failure instead of
crashing during root creation.

The returned buffer is cleared before construction begins.  Root-node creation
does not publish its buffer until both child-parent links have been updated, so
callers cannot mistake a partially initialized record for a successful result.

The source-level regression is
`tests/automated/check-mrm-root-allocation.py`.

## `mrm-safe-copy-allocation.md`

# Mrm safe value-copy allocation

`Urm__CW_SafeCopyValue` creates owned copies for callback and resource values.
Each primitive, string, vector, compound-string, and wide-character allocation
now has a failure guard. Failed copies leave the original value untouched and
return to the caller instead of dereferencing a null pointer; compound-string
vector copies also release earlier entries when a later copy fails.
Wide-character size multiplication and the final Xt `Cardinal` byte width are
checked before allocation as well.

Compound-string vector allocation widens the element count before multiplying
by the pointer size, so valid large counts cannot wrap a signed intermediate.

The source-level regression is
`tests/automated/check-mrm-safe-copy-allocation.py`.

## `mrm-safe-copy-null-sources.md`

# Mrm safe-copy null sources

`Urm__CW_SafeCopyValue` now rejects missing source pointers before copying
callback strings, vectors, floating-point values, or wide-character data.
Zero-valued scalar arguments keep their existing behavior because the checks
apply only to pointer-backed representations.

For a Char8 vector, the copy span is authoritative: a nonempty span requires a
source table even when its item count is zero. This prevents a malformed empty
table from reaching the raw byte copy.

The source-level regression is
`tests/automated/check-mrm-safe-copy-null-sources.py`.

## `mrm-set-widget-class-bounds.md`

# Mrm auto-child class-string bounds

The auto-child (`UrmSetWidgetInstance`) path now applies the same bounded
class-string validation as ordinary widget creation.  A nonzero class offset
must point inside the widget record and terminate before the record ends,
preventing later resource conversion from scanning malformed UID data.

The source-level regression is
`tests/automated/check-mrm-set-widget-class-bounds.py`.

## `mrm-setvalues-input.md`

# Mrm setter input validation

`UrmFetchSetValues` interprets each `Arg.value` as the name of a literal in a
UID hierarchy. The setter now rejects a nonzero argument count with a null
argument array, and rejects null literal-index values before measuring or
copying them. Existing valid argument lists follow the same fetch and
conversion path; malformed input returns `MrmFAILURE` instead of faulting in
`strlen` or `strcpy`.

The source-level regression is `tests/automated/check-mrm-setvalues-input.py`.

## `mrm-string-append-bounds.md`

# Mrm string append bounds

Mrm ordinary string appends now keep `strlen` results in `size_t` and reject
terminator-inclusive strings that exceed `MrmMaxResourceSize` before passing the
length to the 16-bit record allocator. Valid strings retain their existing
storage and offset behavior.

The source-level regression is `tests/automated/check-mrm-string-append-bounds.py`.

## `mrm-string-bounds.md`

# Mrm literal-index string bounds

Mrm value updates now retain literal-index lengths in `size_t` and check the
resource-descriptor addition before allocation. Long or unrepresentable literal
names return `MrmFAILURE` without narrowing the length; ordinary UID value
updates are unchanged.

The source-level regression is `tests/automated/check-mrm-string-bounds.py`.

## `mrm-swap-error-bounds.md`

# Mrm byte-swap diagnostic bounds

Mrm byte-swapping reports malformed record, callback, and widget-resource
types through fixed 300-byte diagnostic buffers. Those paths now use
`snprintf` with the destination size, preserving the existing error messages
while preventing an unexpectedly large record value from overflowing the
stack buffer.

The source-level regression is
`tests/automated/check-mrm-swap-error-bounds.py`.

## `mrm-table-fixup-inputs.md`

# Mrm compression-table fixup bounds

`Urm__FixupCompressionTable` converts file offsets into in-memory pointers before
class and resource lookups use a compression table. It now rejects a null table
and a table whose entry count cannot contain the reserved entries before
walking its entries. Valid tables follow the existing byte-swap and fixup path
unchanged.

## `mrm-table-offset-bounds.md`

# Mrm compression-table offset bounds

Compression-table entries contain offsets into the table's trailing string
storage. Fixup now computes the minimum byte offset after the complete entry
array and rejects entries that point back into the header or entry metadata.
This prevents malformed UID data from treating table metadata as a string while
leaving valid string offsets unchanged.

## `mrm-text-vector-bounds.md`

# Mrm text-vector bounds

Mrm Char8 and CString vector setters now reject negative counts and counts
whose complete text-vector descriptor cannot fit in the format's 16-bit
`MrmSize` field. Zero-count vectors and valid existing counts retain their
behavior, while wrapped `vecsiz` values are prevented before record allocation.

The source-level regression is `tests/automated/check-mrm-text-vector-bounds.py`.

## `mrm-time-output.md`

# Mrm timestamp output validation

The internal timestamp helper now returns immediately when given a null output
buffer, avoiding a crash in malformed header-construction calls. Valid calls
retain the existing platform timestamp formatting and destination contract.

The source-level regression is `tests/automated/check-mrm-time-output.py`.

## `mrm-topmost-index.md`

# Mrm interface-module topmost indices

`UrmIFMSetTopmost` now rejects null or overlong indices before touching the
fixed-width descriptor field.  Valid calls replace the previous index instead
of appending to it, preventing repeated updates from overflowing the field and
making the setter behave as its name indicates.

The source-level regression is
`tests/automated/check-mrm-topmost-index.py`.

## `mrm-validation-macros.md`

# Mrm validation macro null safety

The public Mrm validation macros for hierarchies, class descriptors, widget
records, and interface modules now return false for null pointers before
reading validation fields. Existing valid and invalid non-null records retain
their previous results, while callers avoid null-pointer crashes.

## `mrm-value-bounds.md`

# Mrm values must hold the bytes that are read from them

## Compound strings

A compound string in a UID file is the external byte stream of
`XmCvtByteStreamToXmString`: three ASN.1 header bytes, a length (one byte below
`0x80`, otherwise `0x82` and two bytes) and that many bytes of components. The
converter has no length parameter and reads as many bytes as the header
declares, so a stream whose header declares more than the literal or the widget
record holds was read past its buffer.

`Urm__CW_ValueFits` (which replaces `Urm__CW_UnitsValueFits`) now also checks a
`CString` value: the header and the declared length must lie inside the bytes
that the literal or the widget record holds from the start of the value. It is
applied in `Urm__FetchLiteral` (`MrmFetchLiteral`), in `Urm__CW_ReadLiteral`
(`MrmFetchSetValues`, widget argument lists, callback fixup) and, for values
inside a widget record, in the argument list and callback fixup. A stream that is
too short is a corrupt file: the fetch fails for that literal
(`MrmNOT_VALID` inside the library, `MrmFAILURE` from `MrmFetchLiteral`) and the
argument is not applied. Streams that fit are converted as before.

## Colours

A colour literal (`RGMColorDesc`) is either a name that ends in a NUL character
or three 16 bit components. `Urm__CW_ValueFits` checks that the name ends inside
the literal or the widget record and that an RGB colour holds all components,
before the colour is allocated. A malformed colour fails the fetch
(`MrmFetchColorLiteral`, `MrmFetchSetValues`, widget arguments) like the other
malformed literals.

## Test

```text
python3 tests/automated/run-mrm-value-bounds.py RESULTS
python3 tests/automated/run-mrm-value-bounds.py RESULTS2 --source-revision HEAD
```

`check-mrm-value-bounds.c` writes UID files with the resource manager's own
writer and reads them with `MrmFetchSetValues` and `MrmFetchLiteral` under
AddressSanitizer: colours with an unterminated name or too few components, and streams one byte short, half, or cut inside the header (short
and three byte length) fail cleanly, and exact-size streams convert. The crash
input is kept in `tests/fuzz/corpus/mrm-uid`.

The fuzz stand-in `XmCvtByteStreamToXmString` of `tests/fuzz/mrm-stubs.c` now
reads the length in the two forms of `lib/Xm/XmString.c`, and the class records
of the widget stand-ins carry their class name like real class records.

## `mrm-vector-bounds.md`

# Mrm vector-copy bounds

Mrm vector value copying rejects negative counts and byte sizes before creating
CString or Char8 vector copies, and checks the vector-count multiplication
against both `size_t` and Xt's `Cardinal` width. Invalid vector metadata leaves the source value untouched;
valid UID vectors retain their existing copy behavior.

The source-level regression is `tests/automated/check-mrm-vector-bounds.py`.

## `mrm-wchar-append-bounds.md`

# Mrm wide-string append bounds

Mrm wide-string record appends now count characters and compute byte lengths in
`size_t`, rejecting strings whose terminator-inclusive storage exceeds the
format's `MrmMaxResourceSize` before narrowing to `MrmSize`.

The source-level regression is `tests/automated/check-mrm-wchar-append-bounds.py`.

## `mrm-wci-hash-allocation.md`

# Mrm widget-class hash allocation

Widget-class hash insertion now rejects null table/name inputs, checks the
length before narrowing to the legacy hash function’s `int` parameter, and
handles entry-size overflow or allocation failure before linking a new node.
Valid registration and lookup ordering remain unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-hash-allocation.py`.

## `mrm-wci-hash-callers.md`

# Mrm hash-table caller failure handling

Widget-class and resource-name registration now handle hash-entry allocation
failure explicitly. Class registration removes its just-created descriptor
before returning, while resource-name registration rejects invalid arrays and
stops on a failed insertion instead of dereferencing a null entry. Successful
registration behavior is unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-hash-callers.py`.

## `mrm-wci-hash-helpers.md`

# Mrm hash lookup and deletion inputs

Hash lookup and deletion now reject null tables or names before indexing or
measuring them. Both helpers also reject names that cannot be represented by
the legacy hash function’s `int` length parameter. Valid lookup and deletion
ordering remains unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-hash-helpers.py`.

## `mrm-wci-hash-init.md`

# Mrm hash-table initialization inputs

Hash-table initialization now returns immediately for a null table or
initialization flag, avoiding malformed-call dereferences. Valid one-time
initialization continues to clear the table and set its flag as before.

The source-level regression is `tests/automated/check-mrm-wci-hash-init.py`.

## `mrm-wci-lookup-input.md`

# Mrm name lookup validation

Mrm widget-class and global-name lookup now reject null names and null output
pointers before entering their hash tables. A supplied global-name output is
cleared before an invalid or absent name can return, so callers cannot retain a
stale value after a failed lookup. The class-record lookup also treats a null
name as absent, and its not-found diagnostic uses bounded formatting. Valid
class registration and lookup behavior is unchanged.

The source-level regression is
`tests/automated/check-mrm-wci-lookup-input.py`.

## `mrm-wci-registration-bounds.md`

# Mrm class-registration allocation bounds

Class registration stores the creator name immediately after its descriptor.
The allocation now checks the terminator-inclusive name length and descriptor
addition before calling `XtMalloc`, including Xt's `Cardinal` width on
platforms where `size_t` is wider. This preserves normal registration and
lookup behavior while rejecting an unrepresentable allocation safely.

The source-level regression is
`tests/automated/check-mrm-wci-registration-bounds.py`.

## `mrm-wcr-output-pointers.md`

# Mrm widget-record output pointers

The widget-record string append helpers now reject a missing context or output
offset before handling NULL and empty input strings. The shared space helper
also validates both output pointers before inspecting the resource context.
This preserves successful append behavior while preventing NULL output-pointer
crashes.

The source-level regression is `tests/automated/check-mrm-wcr-output-pointers.py`.

## `mrm-widechar-allocation.md`

# Mrm wide-character allocation handling

Wide-character literal fixup now checks the initial conversion buffer and the
resize result before publishing the pointer.  Failed conversion allocations
return `MrmFAILURE` and release owned storage instead of dereferencing or
losing an unchecked pointer.

The source-level regression is
`tests/automated/check-mrm-widechar-allocation.py`.

## `mrm-widechar-bounds.md`

# Mrm wide-character bounds

Mrm wide-character value fixup now rejects negative serialized byte counts
before converting them to `size_t`, and checks the terminating wide-character
allocation against both the host `SIZE_MAX` and Xt's `Cardinal` (`UINT_MAX`)
width. Valid wide-character values retain their existing conversion and
storage behavior.

The source-level regression is `tests/automated/check-mrm-widechar-bounds.py`.

## `mrm-widget-context-init.md`

# Mrm widget-resource context initialization

The resource loader now checks the context allocation used to fetch a widget
record.  Allocation failure is reported before indexed or RID lookup can use a
null context, while successful widget-resource loading retains its existing
behavior and public interfaces.

The source-level regression is
`tests/automated/check-mrm-widget-context-init.py`.

## `mrm-widget-instance-allocation.md`

# Mrm widget-instance allocation handling

Widget creation and set-values instance builders now check font-list, argument
array, callback-list, and scratch pointer-list setup.  Failure paths release
already-created scratch state before returning, preventing null dereferences
while preserving normal argument construction.

The source-level regression is
`tests/automated/check-mrm-widget-instance-allocation.py`.

## `mrm-widget-instance-input.md`

# Mrm widget-instance input validation

`UrmCreateOrSetWidgetInstance` now rejects a nonzero override count with a
null override array and validates the output/list pointers required by both
create and auto-child dispatch paths before dereferencing them.

The source-level regression is
`tests/automated/check-mrm-widget-instance-input.py`.

## `mrm-widget-name-allocation.md`

# Mrm widget-name allocation failure

`UrmSetWidgetInstance` now checks the temporary name buffer returned by
`ALLOCATE_LOCAL` before writing the lookup prefix. Allocation failure is
reported through the existing Mrm error path instead of causing a null-pointer
dereference.

The source-level regression is
`tests/automated/check-mrm-widget-name-allocation.py`.

## `mrm-widget-name-bounds.md`

# Mrm widget-name lookup bounds

Widget-instance lookup prepends `*` to an uncompressed class name for
`XtNameToWidget`. The temporary buffer now checks the terminator-inclusive
size arithmetic and copies the prefix and string directly, preserving the
lookup spelling while avoiding formatted-output overflow and size wrapping.

The source-level regression is
`tests/automated/check-mrm-widget-name-bounds.py`.

## `mrm-widget-name-record-bounds.md`

# Mrm widget-name record bounds

Mrm widget creation now validates serialized class and widget-name strings
before handing them to class lookup or the widget creator.  Offsets must stay
inside the widget record and have a terminating NUL within the record.  An
explicit override name remains supported without requiring the serialized name,
so existing override behavior is unchanged.

The source-level regression is
`tests/automated/check-mrm-widget-name-record-bounds.py`.

## `mrm-widget-offset-bounds.md`

# Mrm widget record offset bounds

`UrmCreateWidgetInstance` and `UrmSetWidgetInstance` now verify nonzero class
and creation-callback offsets fall within the widget record before using them.
Valid records retain their existing class lookup and callback behavior.

The source-level regression is
`tests/automated/check-mrm-widget-offset-bounds.py`.

## `mrm-widget-override-counts.md`

# Mrm widget override counts

Widget creation and automatic-child set-values accept a public Xt `Cardinal`
override count.  Their temporary allocation accounting now retains that width
as `size_t` and uses a `Cardinal` copy index, rather than first narrowing the
count to Mrm's 16-bit on-disk `MrmCount` type.  Checked aggregate capacity
arithmetic remains in place before the argument-array allocation, so large
valid override lists either allocate safely or fail cleanly without wrapping
the list size.

The focused regression is
`tests/automated/check-mrm-widget-override-counts.py`.

## `mrm-widget-read-null.md`

# Mrm widget-read null-record guards

The hierarchy, indexed-widget, and RID-widget readers now reject a null
context buffer before the byte-swapped-record fallback reads its validation
field. Malformed or empty resource data returns the existing bad-widget-record
error instead of dereferencing a null pointer.

## `mrm-wref-allocation.md`

# Mrm widget-reference allocation handling

Set-values widget-reference and callback-reference builders now check pointer
list initialization, descriptor/name allocation, callback-copy allocation, and
pointer-list append results.  Failure paths release allocations they own while
leaving compressed tag-name storage untouched.

The source-level regression is `tests/automated/check-mrm-wref-allocation.py`.

## `mrm-wref-error-bounds.md`

# Mrm widget-reference diagnostic bounds

Set-values widget-reference construction reports unknown compressed argument
tags through a fixed 300-byte diagnostic buffer. Both widget and callback
reference paths now use `snprintf` with the destination size, preserving the
existing error handling while preventing an oversized tag value from
overrunning the stack buffer.

The source-level regression is
`tests/automated/check-mrm-wref-error-bounds.py`.

## `mrm-wref-growth.md`

# Mrm widget-reference growth

Widget-reference contexts now account for an empty reference vector without
unsigned underflow and grow until the complete descriptor and name heap fit.
Requests that exceed the resource-size or integer limits return `MrmTOO_MANY`
before resizing or copying the context.

The source-level regression is `tests/automated/check-mrm-wref-growth.py`.

## `mrm-wref-input.md`

# Mrm widget-reference input validation

`Urm__CW_AddWRef` now validates its resource context and widget-name pointer
before reading the context buffer or measuring the name. Valid references
retain their existing storage and lookup behavior; malformed input returns
`MrmFAILURE` instead of faulting during reference construction.

The source-level regression is `tests/automated/check-mrm-wref-input.py`.

## `mrm-wref-list-input.md`

# Mrm set-values reference list inputs

Set-values widget and callback reference builders now reject a null output-list
pointer before attempting initialization. The widget-reference path also
rejects a null widget name, preserving valid descriptor creation while
avoiding malformed-call crashes.

Deferred set-values update and resolution now also tolerate a null list or
empty list; resolution rejects a null current widget name before comparing it
with saved references.

The source-level regression is
`tests/automated/check-mrm-wref-list-input.py`.

## `mrm-wref-storage-bounds.md`

# Mrm widget-reference storage bounds

Widget references store names in a 16-bit resource-context layout. The
construction path now measures names in `size_t`, rejects names beyond the
format limit before narrowing, checks full-word alignment arithmetic, and
avoids subtracting a larger used-size value from the context capacity.
Valid references retain their existing lookup and ownership behavior; an
unrepresentable name returns `MrmTOO_MANY` safely.

The source-level regression is
`tests/automated/check-mrm-wref-storage-bounds.py`.

## `mrm-wref-tagname-input.md`

# Mrm set-values tag-name input validation

Set-values widget and callback references copy an application-provided tag name
when its compressed code is unknown. Both paths now reject a null tag name,
check terminator-inclusive sizing against host `size_t` and Xt's `Cardinal`
width, and handle allocation failure before copying. Valid tag names retain
their existing descriptor contents and list behavior.

The source-level regression is
`tests/automated/check-mrm-wref-tagname-input.py`.

## `mrm-writer-arg-value-rebind.md`

# Mrm writer arg-value rebinds

Each type-specific branch in `UrmCWRSetArgValue` now propagates the
argument-descriptor rebind result after record growth.  A failed rebind stops
before writing the serialized offset, while successful type conversions retain
their existing behavior.

The source-level regression is
`tests/automated/check-mrm-writer-arg-value-rebind.py`.

## `mrm-writer-arg-value-validation.md`

# Mrm writer arg-value validation

`UrmCWRSetArgValue` now propagates invalid-context and argument-binding failures
before writing the argument representation type. Valid values retain their
existing type-specific serialization and conversion behavior.

The source-level regression is
`tests/automated/check-mrm-writer-arg-value-validation.py`.

## `mrm-writer-arglist-validation.md`

# Mrm writer arglist validation

`UrmCWRInitArglist` now propagates invalid-context failures before sizing or
allocating an argument descriptor.  Valid contexts retain the existing
argument-count limits and UID serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-arglist-validation.py`.

## `mrm-writer-callback-arg-validation.md`

# Mrm writer callback-argument validation

`UrmCWRSetArgCallback` now rejects null descriptor-offset output storage and
propagates context and argument-binding failures before callback allocation and
after record growth.  Valid callback descriptors retain their existing UID
serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-callback-arg-validation.py`.

## `mrm-writer-callback-routine.md`

# Mrm writer callback routine input

`UrmCWRSetCallbackItem` now rejects a null routine name before measuring or
copying it.  Empty names continue to return the existing `MrmNULL_ROUTINE`
error, while valid callback names retain their normal UID writer behavior.

The source-level regression is
`tests/automated/check-mrm-writer-callback-routine.py`.

## `mrm-writer-char8-vector-rebind.md`

# Mrm writer Char8-vector rebind failures

`UrmCWRSetArgChar8Vec` now checks the argument descriptor rebind immediately
after allocating the vector, as well as after each appended string. Invalid
argument-list state therefore fails before the vector header is dereferenced.

The source-level regression is
`tests/automated/check-mrm-writer-char8-vector-rebind.py`.

## `mrm-writer-char8-vector-validation.md`

# Mrm writer char8-vector validation

`UrmCWRSetArgChar8Vec` now propagates context and argument-binding failures
before vector construction and after record growth.  Valid vectors retain the
existing size limits and UID serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-char8-vector-validation.py`.

## `mrm-writer-child-index.md`

# Mrm writer child index input

`UrmCWRSetChild` now rejects a null index for indexed children before measuring
or appending it.  Empty indexes continue to return the existing
`MrmNULL_INDEX` error, while RID children and valid indexes retain their normal
UID writer behavior.

The source-level regression is
`tests/automated/check-mrm-writer-child-index.py`.

## `mrm-writer-child-validation.md`

# Mrm writer child validation

`UrmCWRSetChild` now propagates invalid-context failures before reading the
widget record or child descriptor.  Valid child indexes, resource IDs, and
management flags retain their existing UID serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-child-validation.py`.

## `mrm-writer-children-validation.md`

# Mrm writer children validation

`UrmCWRInitChildren` now propagates invalid-context failures before sizing or
allocating the child descriptor.  Valid child counts retain the existing size
limits and UID serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-children-validation.py`.

## `mrm-writer-class-input.md`

# Mrm writer class input

`UrmCWRSetClass` now rejects a null user-defined class name before measuring or
copying it.  Empty names continue to return the existing `MrmNO_CLASS_NAME`
error, while valid class names and predefined class codes retain their normal
behavior.

The source-level regression is
`tests/automated/check-mrm-writer-class-input.py`.

## `mrm-writer-comment-input.md`

# Mrm writer comment input

`UrmCWRSetComment` now rejects a null comment pointer before measuring or
copying the string.  Empty comments continue to clear the widget comment, and
non-empty comments retain the existing UID writer behavior.

The source-level regression is
`tests/automated/check-mrm-writer-comment-input.py`.

## `mrm-writer-compressed-tag-validation.md`

# Mrm writer compressed-tag validation

`UrmCWRSetCompressedArgTag` now propagates invalid-context and argument-binding
failures before writing the compressed tag into the UID record.  Valid tags and
related codes retain their existing serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-compressed-tag-validation.py`.

## `mrm-writer-context-validation.md`

# Mrm writer context validation

`UrmIdbPutIndexedResource` and `UrmIdbPutRIDResource` now validate their
resource context before reading group or type fields. Null or stale contexts
return `MrmFAILURE` instead of being dereferenced; valid contexts retain the
existing `MrmNUL_GROUP` and `MrmNUL_TYPE` results.

## `mrm-writer-creation-callback-validation.md`

# Mrm writer creation-callback validation

`UrmCWRSetCreationCallback` now rejects a null descriptor-offset output pointer
and propagates invalid-context failures before allocating or writing the
callback descriptor.  Valid creation-callback setup retains its existing
serialization and callback layout.

The source-level regression is
`tests/automated/check-mrm-writer-creation-callback-validation.py`.

## `mrm-writer-cstring-vector-rebind.md`

# Mrm writer CString-vector rebind failures

`UrmCWRSetArgCStringVec` appends each element to the resource record. An
append can relocate the record, so the argument descriptor must be rebound
before writing the element offset. The setter now checks that per-element
rebind and returns the failure instead of continuing with stale pointers.

The source-level regression is
`tests/automated/check-mrm-writer-cstring-vector-rebind.py`.

## `mrm-writer-cstring-vector-validation.md`

# Mrm writer compound-string-vector validation

`UrmCWRSetArgCStringVec` now propagates context and argument-binding failures
before vector construction and after record growth.  Valid compound-string
vectors retain the existing size limits and UID serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-cstring-vector-validation.py`.

## `mrm-writer-extra-args-validation.md`

# Mrm writer extra-args validation

`UrmCWRSetExtraArgs` now propagates context validation and argument-descriptor
binding failures before writing the extra-argument count.  Valid records keep
the existing count limits and serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-extra-args-validation.py`.

## `mrm-writer-lookup-errors.md`

# Mrm writer lookup error propagation

Writer APIs now continue only when their pre-insert lookup reports the
documented absence condition. Indexed-resource insertion accepts
`MrmINDEX_GT` or `MrmINDEX_LT`; RID insertion accepts `MrmNULL_DATA`. Other
lookup failures, including malformed records and I/O errors, are returned
without creating a new data entry.

## `mrm-writer-resource-callback-routine.md`

# Mrm writer resource callback routine input

`UrmCWRSetCallbackItemRes` now rejects a null routine name before measuring or
copying it.  Empty names continue to return the existing `MrmNULL_ROUTINE`
error, while valid resource callback names retain normal UID writer behavior.

The source-level regression is
`tests/automated/check-mrm-writer-resource-callback-routine.py`.

## `mrm-writer-resource-ref-validation.md`

# Mrm writer resource-reference validation

`UrmCWRSetArgResourceRef` now propagates context and argument-binding failures
before and after appending its resource descriptor.  Valid resource references
retain their existing serialization and argument layout.

The source-level regression is
`tests/automated/check-mrm-writer-resource-ref-validation.py`.

## `mrm-writer-scalar-input.md`

# Mrm writer scalar inputs

`UrmCWRSetArgValue` now rejects null pointers for floating-point and color-table
values before dereferencing or sizing them.  Integer and null representation
types retain their existing immediate-value behavior, and string helpers keep
their established null-as-empty handling.

The source-level regression is
`tests/automated/check-mrm-writer-scalar-input.py`.

## `mrm-writer-uncompressed-tag-validation.md`

# Mrm writer uncompressed-tag validation

`UrmCWRSetUncompressedArgTag` now propagates context and argument-binding
failures both before and after appending the tag string. Valid tags retain
their existing serialization and argument layout.

The source-level regression is
`tests/automated/check-mrm-writer-uncompressed-tag-validation.py`.

## `mrm-writer-vector-input.md`

# Mrm writer vector inputs

The UID writer now rejects a null string-vector pointer when its element count
is nonzero in both character and compound-string setters.  Empty vectors keep
their existing behavior, and individual null elements remain valid inputs for
the existing append helpers.

The source-level regression is
`tests/automated/check-mrm-writer-vector-input.py`.

## `mrm-zero-counts.md`

# Mrm zero-count descriptor inputs

Mrm widget-record argument-list builders reject zero-count inputs before
computing their one-entry-minus-one descriptor sizes. Interface modules may
legitimately contain zero top-level widgets; that case uses a header-only
descriptor instead of subtracting one. This prevents unsigned underflow and
oversized context growth while preserving valid empty and nonempty UID records.

The source-level regression is `tests/automated/check-mrm-zero-counts.py`.

## `multilist-argument-merge.md`

# MultiList argument-list merging

`XmMultiList` combines application-supplied arguments with small, internal
argument lists while it creates its title, scrollbars, search button, frame,
and list child.  Those combinations now reject a `Cardinal` count that would
wrap and treat a failed merged-list allocation as a missing optional child.
An empty title with no arguments remains valid: an empty argument list is
passed through as such.

The focused source contract is covered by
`tests/automated/check-multilist-argument-merge.py`.

## `mwm-action-argument-allocation.md`

# MWM action-argument allocation bounds

MWM parses `f.action` arguments from its configuration file into a dynamically
grown `DtActionArg` array.  The array now has to fit both native `size_t` and
Xt's `Cardinal` allocation size before `XtRealloc` is called.  An oversized
configuration therefore stops argument growth safely instead of truncating the
requested allocation on platforms where `Cardinal` is narrower than `size_t`.

The focused source contract is covered by
`tests/automated/check-mwm-action-argument-allocation.py`.

## `mwm-action-parameter-bounds.md`

# MWM action-parameter bounds

When MWM invokes an action, it records the originating client name and window
title in an optional `name=...[,title=...]` metadata string.  The builder now
keeps the length arithmetic in `size_t`, checks the `Cardinal` allocation limit,
and copies each component into the exact-sized buffer.  Oversized metadata or
allocation failure rejects the parsed action instead of allowing an integer
wrap, an undersized allocation, or a null-pointer write.

## `mwm-active-icon-attributes.md`

# MWM active icon attributes

Activating the optional icon label copies the icon frame's event mask to the
label window. The show path now checks `XGetWindowAttributes` before reading
that mask, so an icon frame destroyed during activation causes a clean return
instead of an invalid attribute update.

## `mwm-active-icon-geometry.md`

# MWM active icon geometry queries

The active icon label is positioned from the icon frame's current geometry.
Both show and move operations now require `XGetGeometry` to succeed before
using the returned coordinates. If the frame has disappeared or become
unqueryable, the optional label operation returns without changing state.

## `mwm-active-icon-label-lifecycle.md`

# MWM active icon-label lifecycle

The active icon label is an optional helper window. Its hide and move paths
now validate the selected screen/client and use a single resolved window ID.
This matters when allocation failed, a screen is being torn down, or cleanup
is requested without an explicit screen pointer: no context or X11 window
operation is attempted with `None`.

## `mwm-active-icon-label-painting.md`

# MWM active icon-label painting

`PaintActiveIconText` draws the optional label's shadow and text. It now checks
the client/screen pointers and helper-window ID before reading appearance data,
so exposure callbacks after helper-window teardown cannot issue invalid X
requests.

## `mwm-active-icon-label-show.md`

# MWM active icon-label show preconditions

Showing the optional active icon label requires both the helper label window
and the icon frame to exist. `ShowActiveIconText` now checks the client and
screen pointers plus both window IDs before querying frame attributes or
reparenting/configuring the label. This keeps a failed icon allocation or
teardown from producing a `BadWindow` request.

## `mwm-active-icon-label-window.md`

# MWM active icon-label window allocation

The large active icon label is optional and is created lazily by
`CreateActiveIconTextWindow`. If X11 cannot allocate that window, the function
now records no label parent and returns. `ShowActiveIconText` consequently
skips the optional label without issuing reparent, configure, map, or context
requests against `None`.

## `mwm-adopt-workspace-bounds.md`

# MWM initial-client workspace bounds

When WSM adopts already-managed windows, it builds an ancillary-window array
containing two fixed windows plus one entry per workspace.  The count is now
validated before the signed addition and `XtMalloc` size calculation.  Invalid
or unrepresentable workspace counts terminate adoption cleanly instead of
allowing integer or allocation-size overflow. If the ancillary array cannot be
allocated, adoption now returns after requesting shutdown rather than assuming
the shutdown hook cannot return. XQueryTree traversal indexes also use its
returned unsigned count type.

## `mwm-appearance-resource-widget.md`

# MWM appearance resource widget

Appearance initialization creates a temporary `client` widget so Motif can
resolve `client.title` resources.  If that widget cannot be allocated, MWM
now reports the startup failure and exits before querying or destroying a null
widget.  Successful resource resolution is unchanged.

## `mwm-application-context.md`

# MWM application context

MWM now checks the Xt application-context allocation before registering
converters or opening the display.  If the context cannot be created, startup
stops through the existing fatal-error path instead of passing a null context
to Xt initialization APIs.  Normal startup is unchanged.

## `mwm-application-shells.md`

# MWM application shells

MWM now validates creation of its application shell and, in workspace-manager
builds, the secondary display connection and shell.  Failed creation stops
startup before color-object initialization, event registration, or resource
updates can use a null shell.  Successful shell classes and resources remain
unchanged.

## `mwm-bevel-geometry.md`

# MWM bevel primitive geometry validation

The shared `BevelRectangle()` and `BevelDepressedRectangle()` primitives now
validate their inputs before doing unsigned geometry arithmetic.  They return
without drawing when either rectangle list is missing, the box is empty, bevel
widths cannot fit, width sums could overflow, or a depressed inset is wider
than one of its four bevel sides.

Valid callers retain the existing shadow-list output.  This protects all MWM
frame and gadget drawing paths from malformed dimensions without changing the
public Motif interfaces.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-bevel-geometry.py
```

## `mwm-binding-buffer-lifetime.md`

# Mwm builtin binding buffer lifetime

When default key or button bindings are expanded with the configured root-menu
name, `mwm` allocates a temporary parser string.  The legacy setup path never
released either string, so repeated per-screen initialization leaked memory; an
allocation failure also reached `sprintf` with a null pointer.

The setup now treats allocation failure as a fatal initialization error and
releases each temporary string immediately after parsing.  Binding text and
parser behavior are unchanged.

`tests/automated/check-mwm-binding-buffer-lifetime.py` protects this contract.

## `mwm-binding-token-boundaries.md`

# MWM binding replacement token boundaries

`FixupBindingsString` replaces vendor key names in the root-window virtual-key
bindings property. The original-name length is kept in `size_t` so a long
property cannot narrow the length before suffix and allocation arithmetic are
checked.

A replacement at the first byte of the property is a valid token. The
boundary check therefore accepts `ks_ptr == fixed_str` without evaluating
`ks_ptr - 1`; forming that pointer would refer before the allocated string and
is undefined behavior. Replacements elsewhere still require a preceding
space, tab, or `>` and a following whitespace boundary, preserving the
existing token-matching rules.

The property writer also rejects strings longer than Xlib's signed `int`
item-count parameter instead of allowing an implicit narrowing conversion.
Allocation failure while copying the input property returns `NULL` before the
replacement loop starts.

The source contract is checked by
`tests/automated/check-mwm-binding-string-growth.py`.

## `mwm-bindings-path.md`

# Mwm binding-path bounds

The Linux `mwm` startup path loads `.motifbind` from the user's home directory.
The legacy code copied that directory into a fixed `MAXWMPATH+1` array before
appending the filename, so unusually long home paths could truncate or
overwrite the path buffer.

The Linux path now allocates the complete home-directory-plus-filename string,
formats it with a bounded write, and releases it after loading the bindings.
The property and fallback binding behavior are unchanged.

`tests/automated/check-mwm-bindings-path.py` protects this contract.

## `mwm-bindings-property-bounds.md`

# MWM bindings property bounds

When MWM publishes a loaded binding file through Xlib, the byte count is now
kept in `size_t` and checked against Xlib's signed `int` property-length
parameter before the request is made. A missing binding string is also treated
as an unsuccessful load rather than being passed to `strlen()`.

The source contract is covered by
`tests/automated/check-mwm-bindings-property-length.py`.

## `mwm-bindings-property.md`

# MWM bindings property status handling

The startup binding loader reads a root-window property with
`XGetWindowProperty`. The result is usable only when that request returns
`Success`; output fields are not a substitute for the request status when the
server or property access fails. `GetBindingsProperty` now checks the status
before accepting the returned string and frees any returned data on failure.
It also rejects properties with nonzero `bytes_after`, so a bounded fetch never
parses and overwrites a truncated virtual-key binding table.

## `mwm-bitmap-cache-growth.md`

# MWM bitmap-cache growth

`GetBitmapIndex` grows each screen's bitmap cache in fixed-size increments.
Growth now checks both the counter increment and the byte multiplication before
calling Xt. It uses a temporary pointer for `XtRealloc`, so allocation failure
leaves the existing cache and counters intact instead of losing the live
pointer and resetting the cache state.

The allocation and failure-preservation contract is covered by
`tests/automated/check-mwm-image-path-allocation.py`.

## `mwm-bitmap-cache-index-bounds.md`

# MWM bitmap-cache index bounds

`GetBitmapIndex` returns an `int`, while the per-screen bitmap-cache count is
unsigned. It now rejects a null screen record and any cache count that cannot
be represented by the return type before searching or appending. This prevents
large-cache indices from silently wrapping to negative values at the API
boundary.

The guard is covered by
`tests/automated/check-mwm-image-path-allocation.py`.

## `mwm-bitmap-cache-init.md`

# MWM bitmap-cache initialization

The first bitmap lookup grows the per-screen cache from an empty state.  If
that allocation fails, the cache must remain in its empty state so a later
lookup can retry.  The initialization now assigns the pointer and capacity
only after the allocation succeeds; an allocation failure reports the error
and leaves `bitmapCacheSize == 0`.

This preserves normal cache behavior and avoids a permanent null-cache state
after one transient allocation failure.

## `mwm-bitmap-cache-installation.md`

# MWM bitmap-cache installation

The WSM bitmap-cache helper now rejects missing names, `None` pixmaps, and
zero dimensions, and checks `XGetImage` before calling `XmInstallImage`. This
keeps built-in icon-cache setup safe when an X resource is unavailable.

## `mwm-bitmap-cache-metadata.md`

# MWM bitmap-cache metadata consistency

`GetBitmapIndex()` now returns `-1` when a screen reports populated bitmap
cache entries but has no cache allocation, or when its entry count exceeds its
recorded allocation capacity. This prevents the search and insertion paths from
dereferencing inconsistent cache metadata while preserving normal cache lookup
and growth behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-bitmap-cache-metadata.py
```

## `mwm-bitmap-cache-retry.md`

# MWM bitmap-cache allocation retry

`GetBitmapIndex()` now resets `bitmapCacheSize` when the cache is empty but its
allocation is null.  This is the state left by a failed initial allocation;
resetting it lets the next lookup enter the normal creation path and retry
instead of returning `-1` forever.

The populated-cache consistency guard remains in place: a nonzero count with a
null pointer is still rejected.  Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-bitmap-cache-retry.py
```

## `mwm-bitmap-data-cache.md`

# MWM raw-bitmap cache installation

The WSM raw-bitmap helper now validates its cache name, source data, and
dimensions before allocating an `XImage`. Invalid descriptors are ignored
without changing cache state; valid built-in bitmaps retain the existing
installation path.

## `mwm-bitmap-path-bounds.md`

# Mwm bitmap path bounds

`BitmapPathName` now checks every home-directory, bitmap-directory and resolved
bitmap path before copying or appending it to the fixed `MAXWMPATH+1` result
buffer. Overlong paths return the original relative name or `NULL` rather than
truncating or producing an unterminated string. Normal lookup order and
short-path results are unchanged.

The focused source check is
`tests/automated/check-mwm-bitmap-path-bounds.py`.

## `mwm-bitmap-path-failure.md`

# MWM bitmap-path failures

`GetBitmapIndex` reserves a null or empty bitmap name for the built-in icon.
A non-empty name whose path cannot be represented or resolved no longer shares
that null result: MWM reports the requested bitmap as unavailable and returns
failure.  Consequently a too-long configured bitmap path cannot silently
replace the requested icon with the built-in one.

The focused source contract is covered by
`tests/automated/check-mwm-bitmap-path-failure.py`.

## `mwm-cached-label-bitmap-bounds.md`

# MWM cached-label bitmap bounds

`MakeCachedLabelPixmap()` now validates its screen record, menu widget, and
bitmap index before accessing the cache.  Negative or out-of-range indices,
an empty cache, and missing inputs return `None`; valid cached labels retain
their existing color lookup and pixmap reuse behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-cached-label-bitmap-bounds.py
```

## `mwm-cached-label-entry-validation.md`

# MWM cached-label entry validation

`MakeCachedLabelPixmap()` now returns `None` for cache entries with a missing
source bitmap or zero width/height.  This prevents invalid X requests while
preserving normal color-specific cache reuse and label-pixmap creation.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-cached-label-entry-validation.py
```

## `mwm-cached-label-resource-failure.md`

# MWM cached-label X resource failure

`MakeCachedLabelPixmap()` now checks both `XCreatePixmap()` and `XCreateGC()`.
If either resource cannot be created, it returns `None`; when the pixmap exists
but the GC fails, it frees that pixmap first.  Successful cache lookup and
label-pixmap generation are unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-cached-label-resource-failure.py
```

## `mwm-callback-safety.md`

# MWM callback ABI safety

`AdjustTearOffControl` returns a `Boolean` when MWM calls it directly to find
out whether menu geometry changed. Xt cascading callbacks, however, use the
void-return `XtCallbackProc` ABI. MWM now registers a void-return adapter that
discards the callback-only result while preserving the direct Boolean call.

This avoids invoking a function through an incompatible pointer type on ABIs
where function return conventions are not interchangeable.

The focused source-contract regression is:

```sh
python3 tests/automated/check-mwm-callback-types.py
```

## `mwm-cci-argument-allocation.md`

# MWM CCI argument allocation

The `f.cci` resource parser creates a modifier/entry record and copies the
entry text.  It now checks the record and every entry-string allocation, and
frees the record on malformed input or allocation failure.  Valid CCI
modifiers and entry text are unchanged.

`tests/automated/check-mwm-cci-argument-allocation.py` protects this contract.

## `mwm-cci-label-prefix.md`

# MWM CCI label prefixing

Legacy CCI modifiers prepend `->`, `=`, `=>`, or `~` to a menu label. The
prefix operation now checks the combined size against both `size_t` and Xt's
`Cardinal` width, allocates one replacement, and swaps it only after the copy
succeeds. The original label remains intact if allocation fails, and valid
modifier behavior is unchanged.

`tests/automated/check-mwm-cci-label-prefix.py` protects this contract.

## `mwm-cde-wsm-window-helper.md`

# MWM CDE session-manager window lookup

When MWM is built with its optional Workspace Services Manager (WSM) support,
it obtains the CDE **session-manager** window before it sends the
`_DT_SM_WM_PROTOCOL` readiness and acknowledgement messages:

```c
_DtGetSmWindow(display, root, &sm_window);
```

The distinction matters: CDE's `_DtGetMwmWindow` finds the **window-manager**
window and is used by a session manager when it needs to contact MWM. MWM's
`wmGD.dtSmWindow` is the inverse endpoint; it is used to send session-manager
protocol messages. CDE's own `dtwm/WmInitWs.c` uses `_DtGetSmWindow` for this
field and includes `Dt/SessionM.h` for its declared prototype. MWM does the
same when optional WSM support is enabled.

`Dt/SessionM.h` is a CDE header that the toolkit does not ship, so MWM includes
it together with its other `Dt/` headers inside the `#ifdef WSM` block and
nowhere else. The standalone window manager (`MWM_WSM`, with its own
`WmWsmLib`) needs no CDE header at all; an include outside that block stopped
`clients/mwm` from compiling without CDE installed.
`tests/automated/run-mwm-standalone-compile.py --build-root BUILD --out OUT`
compiles every MWM translation unit with the exact flags of a configured build
tree and fails if any of them needs a header the toolkit does not provide.

Both helpers return the Xlib property-query status and write their output only
on a successful lookup. MWM retains that established optional-WSM startup
behavior; it does not redirect acknowledgements to its own manager window when
no session manager is running.

MWM sends its one `_DT_WM_READY` notification only after its initial screens,
internal shell, IPC endpoint, decorations, workspace properties, and keyboard
focus have been initialized. This matches CDE dtwm: a legacy
`PANACOMM`-only early send would have produced a duplicate ready message before
that initialization was complete. CDE session managers therefore see one
authoritative final readiness signal regardless of that legacy compile flag.

The focused source regression is
`tests/automated/check-mwm-cde-wsm-window-helper.py`.

## `mwm-client-adoption-inputs.md`

# MWM client adoption input guard

`AdoptClient()` now returns immediately for a null `ClientData` pointer.  The
normal path still updates the save set, applies the client border and geometry,
and reparents the client into its frame.  This protects asynchronous or
partially torn-down lifecycle callbacks without changing valid-client behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-client-adoption-inputs.py
```

## `mwm-client-base-window-initialization.md`

# MWM client base-window initialization

`clientBaseWin` is created only after the frame and gadget geometry are ready.
`GetClientInfo` now initializes it to `None`, so frame cleanup and regeneration
can reliably distinguish an uncreated base window from an arbitrary heap value.

## `mwm-client-creation-failure-cleanup.md`

# MWM client-creation failure cleanup

`GetWmClientInfo` allocates a client record before creating icon-box or
confirmation-window state. If either creation or geometry discovery fails
before the client is registered, the function now releases that record before
returning `NULL`.

## `mwm-client-frame-state.md`

# MWM client-frame state changes

Active and inactive frame transitions update both the frame and, when
decoupled, its title child. Both entry points now require a realized frame and
only touch the title child when it exists, keeping focus/appearance callbacks
safe during partial teardown.

## `mwm-client-icon-shape-initialization.md`

# MWM client icon and shape initialization

`iconMask` is populated from WM_HINTS, while `wShaped` is updated by the X
Shape event path. Both can be absent before the first frame is constructed.
`GetClientInfo` now initializes the mask to `None` and the shape flag to
`False`, preventing early frame/icon paths from consuming indeterminate state.

## `mwm-client-list-growth.md`

# MWM client-list growth

Session-management discovery validates its public output pointers and skips
empty screen-list entries before examining a client. Growth rejects a null
client, validates the count, byte multiplication, and Xt's Cardinal width, and
leaves the existing list untouched if XtRealloc fails. Valid additions retain
their order and count behavior.

`tests/automated/check-mwm-client-list-growth.py` protects this contract.

## `mwm-client-list-inputs.md`

# MWM client-list inputs

`SetMwmClientList` now rejects the invalid combination of a nonzero client
count and a `NULL` window-list pointer before calling `XChangeProperty`.

Valid nonempty lists continue to update `_MWM_CLIENT_LIST` normally. A zero
count is still passed through so callers can clear the property without
allocating a dummy array.

## `mwm-client-sort-bounds.md`

# MWM client-list sorting bounds

Session-management client sorting now skips invalid null lists and passes its
positive signed client count to qsort as an explicit size_t. Its comparator also
orders missing client, screen, or active-workspace state after valid entries
without dereferencing incomplete records. Valid lists retain the same workspace
ordering while malformed inputs are harmless.

The source contract is covered by
`tests/automated/check-mwm-client-sort-bounds.py`.

## `mwm-client-title-initialization.md`

# MWM client title initialization

Client and icon titles are populated from window-manager properties and may be
replaced during later property notifications. `GetClientInfo` now initializes
both XmString handles to `NULL`, ensuring cleanup and replacement paths remain
safe when a client has no usable title property.

## `mwm-colormap-lifecycle-inputs.md`

# MWM colormap lifecycle inputs

Colormap focus, installation, and reset helpers now fail closed for incomplete
inputs:

- Focus and install helpers return when screen data is missing.
- `ResetColormapData` returns for a negative count or for a positive count
  without a window array.
- `ProcessColormapList` requires both screen and client records.

Valid focus changes, colormap installation, and list resets retain their
existing behavior.

## `mwm-colormap-menu-inputs.md`

# MWM colormap and menu getter inputs

`FindColormap` now returns `None` for a missing client record, and
`GetMwmMenuItems` returns `NULL` before accessing the client window. Normal
colormap lookup and `_MWM_MENU` parsing remain unchanged.

The focused source contract is covered by
`tests/automated/check-mwm-colormap-menu-inputs.py`.

## `mwm-colormap-reference-inputs.md`

# MWM colormap reference inputs

`AddColormapWindowReference` and `RemoveColormapWindowReference` now return
immediately for a `NULL` client record. This prevents adding a null entry to a
reference list and prevents removal from treating the list terminator as a
client. Valid client references keep their existing behavior.

## `mwm-command-tree-bounds.md`

# Mwm command-tree bounds

Inline menu-command matching derives command paths and function arguments from
user-defined command-tree names.  The legacy matching path placed both strings
in fixed 1024-byte arrays, so long menu names could overflow stack storage.

The path now measures and allocates each rendered string, frees both after the
match is stored, and preserves the measured byte count as `size_t` through the
Xt `Cardinal` boundary. Existing command and cascade behavior is unchanged.

`tests/automated/check-mwm-command-tree-bounds.py` protects this contract.

## `mwm-command-tree-lifecycle.md`

# MWM command-tree lifecycle

Command-branch removal advances its owned node pointer for every sibling and
releases each node's name and default label exactly once. Command deletion uses
pointer-to-pointer traversal so deleting a head entry continues through the
remaining siblings and duplicate IDs, including nested branches.

The previous branch loop retained the first node pointer, freeing it repeatedly
when a command cascade had multiple children. Replacing Wsm's command menus
could consequently crash MWM. Selection-owner recovery also removes commands
belonging to the departed workspace manager on the correct managed screen.

`tests/automated/run-mwm-command-tree-lifecycle.py` runs the production deletion
functions under ASan/UBSan with head, sibling, nested and repeated-ID cases and
checks release of all node/name/label allocations. The native Wsm acceptance
suite exercises replacement sessions, menu regeneration and MWM restart.

## `mwm-command-window-allocation.md`

# MWM command window-list allocation

The MWM include, enable, disable, rename, and remove command handlers now
reject empty window lists and allocation failure before reading or writing the
list. This prevents malformed client-command messages from dereferencing a
null list while leaving valid non-empty command dispatch unchanged.

`tests/automated/check-mwm-command-window-allocation.py` protects this
contract.

## `mwm-command-wire-format.md`

# MWM client-command wire format

MWM's Define, Include, Remove, Enable, Disable, and Rename command-selection
targets use a packed byte stream of CARD values and length-prefixed strings.
They now reject a null payload or a selection request whose format is not
8 bits before the payload reaches the command parser.  Existing correctly
formed clients continue to use format 8 unchanged; malformed 16- or 32-bit
requests fail conversion rather than being interpreted as byte data.

Client-message atom values are likewise range-checked before crossing Xlib's
signed `data.l` boundary, and a negative public message type is rejected.

The focused source contract is covered by
`tests/automated/check-mwm-command-wire-format.py`.

## `mwm-compact-gadget-geometry.md`

# MWM compact system and minimize geometry

`BevelSystemButton()` now returns below five pixels, and
`BevelMinimizeButton()` returns below seven pixels.  These are the minimum
heights supported by their legacy glyphs; rejecting smaller values before the
switch avoids unsigned subtraction wrapping while preserving normal title-bar
rendering.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-compact-gadget-geometry.py
```

## `mwm-compound-string-drawing-inputs.md`

# MWM compound-string drawing input guard

`WmDrawXmString()` now validates its display, font list, compound string,
graphics context, clip box, and active-screen state before calling
`XmStringWidth()` or the Motif drawing routines.  Valid multilingual title and
feedback strings retain their existing alignment, clipping, and clean-text
behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-compound-string-drawing-inputs.py
```

## `mwm-config-allocation-warning.md`

# Mwm include-stack allocation warnings

When an included resource file cannot be added to the WSM configuration stack,
`mwm` reports the included filename.  Both allocation-failure branches used
the shared `wmGD.tmpBuffer`, so long filenames or translations could overflow
that fixed buffer.

A shared warning helper now measures the complete message, checks that the
formatter result fits Xt's `Cardinal` width, and allocates it for the duration
of `Warning`. Both stack initialization and push paths retain their existing
recovery behavior.

`tests/automated/check-mwm-config-allocation-warning.py` protects this contract.

## `mwm-config-reopen-warning.md`

# Mwm configuration-reopen warning bounds

When an included resource file is popped from the configuration stack,
`mwm` reopens the previous file and reports failures with its pathname.  The
legacy diagnostic used a fixed `MAXWMPATH+1` buffer, so a long path or
localized format could overflow it.

The warning now allocates the complete rendered message after checking the
formatter result against Xt's `Cardinal` width and preserves the include-stack
recovery behavior. A short fallback is emitted if message allocation fails.

`tests/automated/check-mwm-config-reopen-warning.py` protects this contract.

## `mwm-config-stack-allocation.md`

# MWM configuration-stack allocation handling

MWM initializes and extends its configuration-file stack transactionally. If
allocation of either a stack entry, its owned file-name copy, or a parser buffer
fails, no partial global stack is published and the parent parser remains open.
The configuration-file opener closes its provisional file and returns failure
rather than allowing the optional preprocessor path to dereference a null stack
entry. Expanded include filenames remain owned by the include parser and are
released whether pushing the child frame succeeds or fails.

The focused source-contract regression check is
`tests/automated/check-mwm-config-stack-allocation.py`.

## `mwm-confirm-action-bounds.md`

# MWM confirmation-action bounds

`ConfirmAction()` now returns for a missing screen record, missing top-level
widget, or an action index outside the four built-in confirmation actions.
Valid confirmation dialogs retain their existing message, callback, centering,
and management behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-confirm-action-bounds.py
```

## `mwm-confirm-callback-state.md`

# MWM confirmation callback state

`OkCB()` now returns safely when callback data is missing or its action number
is outside the four built-in confirmation handlers.  The dialog is still
withdrawn and the mapped flag cleared in those cases; valid callbacks retain
their existing action dispatch.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-confirm-callback-state.py
```

## `mwm-confirmation-border-width.md`

# MWM confirmation-window border width

The confirmation-window path in `GetWmClientInfo` receives the X11 border
width as an `unsigned int`, but the client record stores it as an `int`.
Passing the record field directly to `XGetGeometry` through a cast could write
an out-of-range value and leave a negative border width in later frame
geometry.  The path now uses a correctly typed temporary, rejects values that
cannot be represented by the client record, and releases the unregistered
client record on that failure.

Normal border widths are copied unchanged, so existing confirmation-window
layout remains compatible.

## `mwm-confirmation-geometry-bounds.md`

# MWM confirmation-window geometry bounds

`XGetGeometry` reports the confirmation window's width and height as unsigned
values, while `ClientData` stores them as signed `int` fields.  The
confirmation path now rejects dimensions above `INT_MAX` before copying them
into the client record and before deriving the min/base/max size fields.

This prevents an oversized or malformed X11 geometry reply from turning into a
negative frame size.  Normal confirmation windows retain their existing
geometry and placement behavior.

## `mwm-confirmation-geometry.md`

# MWM confirmation-window geometry

Confirmation windows are internally managed clients, but their X window can
still disappear during setup. `GetWmClientInfo` now requires
`XGetGeometry` to succeed before recording the client dimensions and border.
Failure returns `NULL` with a warning, avoiding partially initialized state.

## `mwm-continuation-buffer-bounds.md`

# MWM continued configuration-line bounds

MWM accepts backslash-continued configuration lines.  The remaining capacity
for each appended `fgets` call is now calculated as `size_t`, checked for an
exhausted buffer and for `int` representability, and only then passed to the C
library.  Oversized or exhausted continuations stop safely rather than relying
on an implicit pointer-difference conversion.

The focused source contract is covered by
`tests/automated/check-mwm-continuation-buffer-bounds.py`.

## `mwm-copyargv-fallback.md`

# MWM secondary-display argument fallback

WSM opens a second display connection using the copied command-line vector.
When copying either vector fails, `CopyArgv` falls back to the caller-owned
`argv`.  The fallback now also sets `dpy2Argc`, keeping the argument count and
vector consistent for `XtOpenDisplay`.

Successful copies retain the existing duplicated storage and argument count.

## `mwm-current-workspace-initialization.md`

# MWM current workspace initialization

WSM client operations temporarily switch `currentWsc` while addressing a
client's per-workspace records. `GetClientInfo` now initializes that index to
workspace zero before any workspace-sensitive operation, preventing an
uninitialized index from selecting outside the workspace list.

## `mwm-db-home-allocation.md`

# MWM database filename allocation

When no session database path is configured, MWM derives one from the home
directory.  The lookup is now checked before measuring or formatting it, and
the combined path length is bounded against both `size_t` and Xt's `Cardinal`
width before allocation.  If the home directory
is unavailable or unrepresentable, MWM leaves the database path unset for the
existing caller fallback instead of dereferencing null or wrapping a size.

`tests/automated/check-mwm-db-home-allocation.py` protects this contract.

## `mwm-db-home-regression.md`

# MWM database-path regression contract

The home-directory fallback test verifies both missing-home handling and the
current checked allocation/formatting path. It intentionally requires the
measured `dbFileLength` allocation and `snprintf` result check, so the test
cannot silently drift back to the removed unbounded formatter.

```sh
python3 tests/automated/check-mwm-db-home-allocation.py
```

## `mwm-debug-resource-path-bounds.md`

# MWM debug resource-dump paths

The debug `dump_resources` function writes one resource database per screen.
The generated `/tmp/dtwm.resources.<screen>` path now uses `snprintf` and is
checked for formatting failure or truncation before it is passed to Xrm. This
keeps the fixed compatibility buffer bounded without changing the established
debug file naming convention.

The focused source contract is checked with:

```sh
python3 tests/automated/check-mwm-debug-resource-path.py
```

## `mwm-decoration-allocation-inputs.md`

# MWM decoration allocation input guards

The frame-decoration setup helpers now reject invalid records before reading
their fields:

- `CountFrameRectangles()` and `InitClientDecoration()` return for a null
  screen record.
- `AllocateFrameDisplayLists()` returns `FALSE` for a null client record.

Normal screen and client records use the existing rectangle counting and
allocation behavior.  The guards protect startup and teardown callbacks from
turning a missing record into a window-manager crash.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-decoration-allocation-inputs.py
```

## `mwm-default-colors-growth.md`

# MWM default-color cache growth

The MWM default-color cache grows as new screen/colormap combinations are
seen. Growth now checks the signed counter and Xt allocation-size limits, and
uses a temporary pointer before publishing the larger cache. Failed growth
therefore leaves the existing cache available to subsequent calls.

The behavior is covered by
`tests/automated/check-mwm-default-colors-allocation.py`.

## `mwm-dialog-context.md`

# MWM dialog context lookups

Dialog lifecycle callbacks recover their client record from an X context. Both
withdraw and remanage paths now require the lookup to return a non-null
`ClientData` pointer before changing lists, focus, or frame state.

## `mwm-dialog-frame-lifecycle.md`

# MWM dialog frame lifecycle

Dialogs can be withdrawn or remanaged while their frame is being created or
torn down. `WithdrawDialog` and `ReManageDialog` now check
`clientFrameWin != None` before unmapping or mapping it, so lifecycle events
remain safe for a partially initialized client.

## `mwm-display-env-bounds.md`

# Mwm display environment bounds

The window manager exports the active X display as `DISPLAY=...` for launched
clients.  The old path copied the X display name through two fixed 256-byte
arrays, so a long hostname, transport endpoint, or display suffix could
overwrite stack storage before the environment value was installed.

`InitWmDisplayEnv` now allocates the persistent environment string from the
complete display-name length and formats it with a bounded write.  The emitted
`DISPLAY=` value and `putenv` lifetime remain unchanged for ordinary displays.
The length calculation now rejects size_t overflow and values that do not fit
Xt's `Cardinal` allocation width before allocating.

`tests/automated/check-mwm-display-env-bounds.py` protects this contract.

## `mwm-display-open-failure.md`

# MWM display-open failure

After `XtOpenDisplay` fails, MWM now returns immediately after its existing
fatal-error call.  This keeps later Xlib and Xt initialization from receiving a
null display if an embedding or test environment allows the error routine to
return.  Successful display initialization is unchanged.

## `mwm-dt-request-inputs.md`

# MWM DT request inputs

`GetDtWmRequest` now returns immediately when its screen data or either output
pointer is missing. For valid calls it initializes `*pszReq` and `*pmore` to a
safe empty result before reading the property, while preserving the existing
request extraction behavior.

An empty or null payload is also rejected before extraction.  That prevents a
type-correct but zero-item X11 property from being converted into a synthetic
empty request and reaching the desktop-request parser.

The bounded X11 fetch must also be complete. A request property with bytes
remaining is discarded and deleted instead of allowing a truncated command to
reach the parser or remain queued indefinitely.

Queued request state belongs to the originating screen, so interleaved
multi-screen requests cannot cross-contaminate their parser input. An
allocation failure while copying a request drops that screen's queued
property rather than retrying it indefinitely.

The focused source contract is covered by
`tests/automated/check-mwm-dt-request-inputs.py` and
`tests/automated/check-mwm-dt-request-empty-property.py`, with queue ownership
covered by `tests/automated/check-mwm-dt-request-per-screen.py`.

## `mwm-embedded-client-growth.md`

# MWM embedded-client property growth

`UpdateEmbeddedClientsProperty` now grows its temporary window-ID list through
a replacement pointer. The count is advanced only after allocation succeeds,
and the existing `_DT_WORKSPACE_EMBEDDED_CLIENTS` property is left untouched
if a growth allocation fails. The previous direct `XtRealloc` assignment could
lose the old list and leave the count out of sync.

Allocation byte arithmetic is checked against both host and Xt limits. The
focused source contract is covered by
`tests/automated/check-mwm-embedded-client-growth.py`.

## `mwm-embedded-client-shutdown.md`

# MWM embedded-client shutdown

Front-panel embedded clients can disappear before MWM shuts down. The
`UnParentControls` shutdown loop now skips records marked `CLIENT_DESTROYED`
before any save-set, reparent, or unmap request. Live embedded clients retain
the existing root-window restoration behavior.

## `mwm-error-bounds.md`

# Mwm error-diagnostic bounds

The window manager's WSM warning wrapper and X I/O error handler receive
arbitrary display, locale, and diagnostic text.  They previously formatted
those values into fixed 1024-byte and 100-byte stack buffers, respectively.

Both paths now measure the complete message, allocate it with Xt memory, and
release it after dispatch.  Warning and fatal-exit behavior remains unchanged;
allocation failure emits a short fallback diagnostic instead of overflowing a
stack buffer.

`tests/automated/check-mwm-error-bounds.py` protects this contract.

## `mwm-error-code-format-bounds.md`

# MWM debug error-code formatting

The debug X-error handler converts the request opcode to text before querying
the X request database. That conversion now uses `snprintf` and clears the
temporary key if formatting reports an error, keeping the diagnostic path
bounded without changing normal error reporting.

The focused source contract is checked with:

```sh
python3 tests/automated/check-mwm-error-code-format.py
```

## `mwm-exclusion-allocation.md`

# MWM menu-exclusion allocation

Menu resource parsing stores exclusion strings for later command matching.
Previously, allocation failure left the parser writing through a null
`MenuExclusion` record or retained a partially initialized record.  The
parser now ignores an exclusion when its record or copied command string
cannot be allocated, and frees the record before returning.  Valid exclusions
keep the same ordering and matching behavior.

The client-command parser also initializes the temporary exclusion-text
pointer and passes it to `StoreExclusion` only after a `~` modifier has
captured a value.  This makes the ownership boundary explicit and prevents a
malformed future parser path from passing an indeterminate pointer.

`tests/automated/check-mwm-exclusion-allocation.py` protects this contract.

## `mwm-exclusion-copy-allocation.md`

# MWM menu-exclusion copy allocation

Custom menus duplicate the exclusion list from their source menu.  The copy
now checks each node and command-string allocation, links only fully
initialized nodes, and releases the partial list on failure.  A failed copy
therefore aborts custom-menu construction cleanly instead of dereferencing a
null node or retaining leaked partial state.  Successful copies preserve list
order and matching behavior.

`tests/automated/check-mwm-exclusion-copy-allocation.py` protects this
contract.

## `mwm-feedback-colormap.md`

# MWM feedback colormap cleanup

The feedback overlay restores colormap focus when it is hidden. During screen
shutdown there may be no active screen, so `HideFeedbackWindow` now performs
that restoration only when `ACTIVE_PSD` exists; unmapping the feedback window
still occurs normally.

## `mwm-feedback-dialog-creation.md`

# MWM feedback dialog creation

Confirmation dialogs are created lazily.  `ConfirmAction` now checks the
popup shell, message box, and required OK, Cancel, and Help children before
using them.  A partial dialog is destroyed and the cached widget is cleared
when any required allocation fails.  Message-string comparison also tolerates
missing toolkit strings.  The normal dialog hierarchy and resources are
unchanged.

## `mwm-feedback-fallback.md`

# Mwm feedback-message fallback

Localized confirmation messages are allocated during feedback-dialog setup.  If
allocation of the quit message failed in a non-`MINIMAL_DT` workspace-manager
build, the legacy fallback stored the text in the temporary catalog pointer
instead of `confirm_mesg[3]`.  The live message pointer remained null and could
later be passed to `XmStringCreateLocalized`.

The fallback now assigns the standard workspace-manager literal directly to
`confirm_mesg[3]`, matching the other allocation-failure branches and avoiding
a pointer into catalog-managed temporary storage.  Normal localized messages
and resource overrides are unchanged.  `CopyFeedbackString` already owns a
successful localization copy, so initialization no longer redundantly copies
that value a second time.

`tests/automated/check-mwm-feedback-fallback.py` protects this contract.

## `mwm-feedback-geometry.md`

# MWM feedback-window geometry

Move/resize feedback text uses signed X coordinates while the feedback window
dimensions are unsigned.  MWM now validates text-derived dimensions before
storing them, performs placement with signed 64-bit intermediates, and clamps
only at the Xlib coordinate boundary.  Text baselines use the same checked
path.  Negative geometry and feedback windows larger than the display therefore
no longer wrap through unsigned subtraction or overflow signed baseline
coordinates.

The focused source contract is covered by
`tests/automated/check-mwm-feedback-geometry.py`.

## `mwm-feedback-update-inputs.md`

# MWM feedback update input guards

`UpdateFeedbackInfo()` and `UpdateFeedbackText()` now reject a missing screen
record; the text helper also rejects a missing feedback font before calling
`XTextExtents()`.  Valid position and size updates retain their existing
formatting and centering.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-feedback-update-inputs.py
```

## `mwm-feedback-window-create.md`

# MWM feedback-window creation failure

The feedback overlay is optional runtime UI. `UpdateFeedbackWindow` now stops
after a failed `XCreateWindow` instead of continuing into configure/map calls
with `None` as the window ID. Normal feedback updates remain unchanged when
the overlay window is available.

## `mwm-feedback-window-lifecycle.md`

# MWM feedback-window lifecycle guards

The feedback-window entry points now handle incomplete lifecycle state safely:

- `ShowFeedbackWindow()` rejects a missing screen or font and stops if either
  reusable bevel list cannot be allocated.
- `PaintFeedbackWindow()` requires a realized window and both bevel lists.
- `HideFeedbackWindow()` returns for a missing screen record.

Valid feedback positioning, sizing, beveling, and text rendering are unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-feedback-window-lifecycle.py
```

## `mwm-font-warning-bounds.md`

# Mwm font-load warning bounds

When a configured render table cannot provide a default font, `mwm` reports the
font list and exits.  The old path formatted that diagnostic into the shared
`wmGD.tmpBuffer` (`MAXBUF` bytes), so a long localized format could overwrite
storage before the fatal exit.

The diagnostic now allocates its measured rendered length and keeps the same
warning and exit behavior.  A short fallback is used if allocation fails.

`tests/automated/check-mwm-font-warning-bounds.py` protects this contract.

## `mwm-frame-construction-inputs.md`

# MWM frame construction input guards

`ConstructFrame()` now returns `FALSE` when its `ClientData` argument is null,
and `GenerateFrameDisplayLists()` returns without drawing.  Both routines are
used during frame setup and redraw; rejecting an invalid lifecycle callback
keeps the window manager alive while preserving the existing behavior for a
valid client record.

The source-contract regression check is:

```sh
python3 tests/automated/check-mwm-frame-construction-inputs.py
```

## `mwm-frame-display-list-counts.md`

# MWM frame display-list counts

When title and resize appearance are shared, `AllocateFrameDisplayLists`
allocates one top and one bottom shadow list containing both kinds of frame
elements. The bottom-list count now combines `NUM_RESIZE_BS_ELEMENTS` with
`NUM_TITLE_BS_ELEMENTS`; the previous code added the resize count twice and
could size the list incorrectly for themes with different title and resize
geometry.

The separate-title path remains unchanged. The source contract is covered by
`tests/automated/check-mwm-frame-display-list-counts.py`.

## `mwm-frame-exposure.md`

# MWM frame exposure drawables

X exposure events may be delivered while MWM is destroying a client frame.
Both frame and base-window exposure callbacks now require a realized drawable
before painting bevel rectangles, keeping late events harmless.

## `mwm-frame-id-lifecycle.md`

# MWM frame-hierarchy IDs

Destroying a client frame also destroys its title and base child windows. The
teardown helper now clears all three cached IDs (`clientFrameWin`,
`clientTitleWin`, and `clientBaseWin`) together, keeping the client record
consistent with the X server and making repeated lifecycle operations safe.

## `mwm-frame-regeneration.md`

# MWM frame regeneration

Frame regeneration can be requested during client teardown or after an
allocation failure. `RegenerateClientFrame` now validates the frame, optional
title/base children, and all eight resize-cursor windows before reconfiguring
them. Partial hierarchies are left untouched instead of producing invalid X
requests.

## `mwm-frame-shape-inputs.md`

# MWM frame-shape input guard

`SetFrameShape()` now returns immediately when its `ClientData` argument is
null.  This protects shape-notify and teardown callbacks before any border,
matte, or shaped-window fields are read; valid shaped and unshaped clients
retain the existing X Shape behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-frame-shape-inputs.py
```

## `mwm-frame-teardown.md`

# MWM frame teardown

Frame construction can fail before a client frame window exists. The teardown
helper `FreeClientFrame` now handles both a null client pointer and a `None`
frame ID, and clears the ID after destruction. This makes cleanup safe after a
partial construction failure and prevents repeated teardown from issuing a
`BadWindow` request.

## `mwm-frame-window-construction.md`

# MWM frame-window construction failure handling

`ConstructFrame` builds the client frame, optional title window, and base
window in that order. X11 can return `None` when a server cannot allocate a
window. The constructor now checks every allocation before using the resource,
and destroys the partial frame hierarchy when a later allocation fails.

This keeps resource-exhaustion failures local: MWM returns `False` to its
caller instead of issuing child-creation, mapping, or drawing requests against
an invalid window ID.

The source guard is covered by
`tests/automated/check-mwm-frame-window-construction.py`.

## `mwm-fscreen-default.md`

# Mwm single-screen navigation index

`F_Screen` keeps the previous screen for the `back` action. With one managed
screen, `wmGD.Screens` is an ordinal array whose only valid index is zero,
while Xlib may report a nonzero default screen number. The initialization now
maps that single-screen case to index zero; multi-screen displays retain the
existing screen-number behavior.

`tests/automated/check-mwm-fscreen-default.py` guards this mapping.

## `mwm-function-arg-bounds.md`

# Mwm menu function-argument bounds

Menu insertion derives cascade names and invoke-command arguments from
user-defined command trees and X11 identifiers.  The legacy insertion path
placed them in a fixed 256-byte buffer, so long cascade names could overflow
stack storage.

Both argument forms now use measured dynamic allocation and are released after
matching completes.  Menu matching and command dispatch content are
unchanged. The formatter result remains signed until negative errors are
rejected, then is checked against Xt's byte-count limit before allocation.
Null command-tree and cascade-name inputs fail safely instead of reaching the
formatter.

`tests/automated/check-mwm-function-arg-bounds.py` protects this contract.

## `mwm-function-argument-bounds.md`

# MWM function-argument bounds

MWM function parsers now retain `strlen()` results in `size_t` until the
allocation bound is checked against Xt's `Cardinal` width. The `F_Exec`
suffix path also handles an empty argument without indexing before the
allocated string. Multibyte byte counts remain `int`, matching `mblen()`.

The source contract is covered by
`tests/automated/check-wm-resparse-function-args.py`.

## `mwm-function-table-indexes.md`

# MWM function-table indexes

MWM function-table scans now use `size_t` indexes, matching the type produced
by `sizeof(functionTable) / sizeof(functionTable[0])`. This removes signed /
unsigned comparison narrowing while preserving the existing final-entry
sentinel and function-table behavior.

The source contract is covered by
`tests/automated/check-wm-function-table-indexes.py`.

## `mwm-gadget-bevel-allocation.md`

# MWM gadget bevel allocation failure

`DepressGadget()` uses two small reusable rectangle lists to paint a title-bar
gadget.  The old short-circuit allocation expression could leave one list null
and still fall through to drawing.  The helper now allocates each missing list
independently and returns `FALSE` when either allocation is unavailable; the
next call can retry the missing list without changing successful drawing.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-gadget-bevel-allocation.py
```

## `mwm-gadget-rectangle-inputs.md`

# MWM gadget-rectangle inputs

Frame-decoration helpers now handle missing client records safely:

- `AllocateGadgetRectangles(NULL)` returns `False`.
- `ComputeGadgetRectangles(NULL)` returns without changing state.

Valid client records use the existing allocation and geometry behavior.

## `mwm-grab-mask-allocation.md`

# MWM grab-mask allocation

`_WmGrabMasks` builds the modifier-mask list used by key and button grabs.
Allocation grows the reusable list with a temporary pointer and returns zero
masks if the replacement fails, leaving the previous buffer intact. The old
path freed the existing list before allocation and then dereferenced a null
pointer after failure.

The helper tolerates an absent lock-mask sequence, bounds the count before
adding terminator slots, and verifies that the requested byte count fits both
native `size_t` arithmetic and Xt's `Cardinal` allocation interface. An
unrepresentable resource leaves the existing cache untouched. Existing
successful grab behavior is unchanged. The source contract is covered by
`tests/automated/check-mwm-grab-mask-allocation.py`.

The implementation includes `<stdint.h>` directly for `SIZE_MAX`; this keeps
the checked path portable to toolchains whose Motif/X11 headers do not happen
to transitively provide that C definition.

The lock-mask sequence builder now also uses unsigned shift operands, rejects
unrepresentable sequence widths before deriving `2^n - 1`, and returns safely
when its initial zeroed allocation fails. The grab helper then follows its
existing absent-sequence fallback.

If Xlib cannot return a modifier mapping, MWM now retains the base Caps-Lock
mask and builds that sequence without dereferencing the absent mapping.

When XKB modifier mappings change, MWM releases the previous sequence before
publishing its replacement, preventing one Xt allocation leak per mapping
refresh.

Before rebuilding a modifier sequence, MWM now refreshes Xlib's keyboard
mapping cache for keyboard and modifier `MappingNotify` events. This keeps the
key-symbol lookup used to discover locking modifiers synchronized after a
layout or modifier change.

## `mwm-graphics-size-limits.md`

# MWM graphics size limits

`ExtendRList` uses `SIZE_MAX` when checking rectangle-array byte counts. The
graphics source now includes `<stdint.h>` explicitly, so that checked arithmetic
does not depend on a transitive system-header include.

## `mwm-group-argument-bounds.md`

# MWM group-argument bounds

MWM group parsing now keeps the scanned pointer distance in `size_t` and
clamps it directly to the fixed group buffer capacity. This avoids narrowing
the distance through the generic integer `min` macro while preserving the
existing accepted group names and truncation limit.

The source contract is covered by
`tests/automated/check-wm-resparse-group-length.py`.

## `mwm-highlight-gc.md`

# MWM highlight graphics-context helper

`GetHighlightGC` obtains bevel graphics contexts through the screen's top-level
widget. It now returns `NULL` when that screen/widget state is unavailable,
avoiding a null dereference during early initialization or teardown.

## `mwm-hotspot-geometry.md`

# MWM hotspot geometry

MWM uses an X11 `XRectangle` for system-menu hotspot hit testing.  Its origin
coordinates are signed 16-bit values, while its width and height are unsigned
16-bit values.  The hit test now widens both coordinates and extents before
forming the exclusive right and bottom edges.

This preserves ordinary hotspot behavior and fixes large (at least 32768-pixel)
hotspots, whose extents previously became negative through a signed `short`
conversion and could fail to receive pointer events.

`tests/automated/check-mwm-hotspot-geometry.py` verifies that the test keeps
the unsigned extent values intact.

## `mwm-icon-bevel-gcs.md`

# MWM icon bevel graphics contexts

When MWM composes a framed custom icon, it obtains temporary top- and
bottom-shadow GCs with `XtGetGC`.  The temporary references are now released
after drawing and if only one acquisition succeeds.  This preserves the
rendering while preventing repeated icon creation from accumulating Xt GC
references.

The focused source contract is covered by
`tests/automated/check-mwm-icon-bevel-gc-release.py`.

## `mwm-icon-box-resources.md`

# MWM icon-box drawing resources

The icon-box GC helpers now handle resource failures safely:

- `MakeShrinkWrapIconsGC()` returns before `XCopyGC()` when its screen or GC is
  unavailable.
- `MakeFadeIconGC()` cleans up partial GCs and stipple pixmaps if any resource
  creation fails, then leaves the screen record disabled for retry.

Successful icon-box fade and shrink-wrap rendering is unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-icon-box-resources.py
```

## `mwm-icon-destination-bounds.md`

# MWM icon destination bounds

`MakeIconPixmap()` now rejects zero maximum icon dimensions and checks that
shadow/slab margins fit in unsigned arithmetic before constructing the
destination pixmap size.  Valid icon dimensions retain the existing margin,
scaling, and bevel behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-icon-destination-bounds.py
```

## `mwm-icon-dimension-types.md`

# MWM icon geometry type boundaries

MWM configuration resources store icon dimensions as signed values, while Xlib
pixmap geometry and copy dimensions are unsigned.  Icon composition now
validates the signed resources before conversion, uses explicit unsigned
dimensions for pixmap operations, rejects source offsets that cannot fit Xlib's
signed coordinate arguments, and validates the display depth before converting
it for pixmap creation.  This preserves normal icon sizing while making
malformed or extreme geometry fail safely.

The focused source contract is covered by
`tests/automated/check-mwm-icon-dimension-types.py`.

## `mwm-icon-event-handlers.md`

# MWM icon-box event handlers

The icon-box event handlers now use Xt's native `XtEventHandler` function
signature directly: widget, client data, `XEvent`, and the dispatch-control
pointer.  This replaces incompatible function-pointer casts that could make
callback invocation undefined on ABI-sensitive platforms.  Key and focus
handlers select their concrete event view from the supplied `XEvent` only after
the normal null checks.

The focused source contract is covered by
`tests/automated/check-mwm-icon-event-handlers.py`.

## `mwm-icon-frame-window.md`

# MWM icon-frame window allocation

When an icon is displayed outside the icon box, `MakeIcon` creates its frame
window before allocating shadow geometry and repainting the icon. The
constructor now checks that allocation for both normal and WSM builds. A
failure returns `False` immediately, so later icon operations never target an
invalid X window.

## `mwm-icon-gc-mask.md`

# MWM icon-box graphics-context masks

The icon-box fade and shrink-wrap graphics contexts copy every GC field from
the inactive icon GC. Their all-bits copy mask is now explicitly expressed in
Xt's unsigned `XtGCMask` type, rather than relying on a signed `long` value
being implicitly converted at the Xlib boundary.

The focused source contract is covered by
`tests/automated/check-mwm-icon-gc-mask.py`.

## `mwm-icon-list-allocation.md`

# MWM icon-list allocation

Icon boxes create and grow their placement list when icon cells are required.
Both paths now validate native `size_t` and Xt `Cardinal` allocation limits,
then compute allocation, copy, and initialization byte counts in `size_t`.
Large valid counts therefore cannot overflow signed intermediate arithmetic
before the request reaches the allocator.

The focused source contract is covered by
`tests/automated/check-mwm-icon-list-allocation.py`.

## `mwm-icon-painting.md`

# MWM icon painting preconditions

Active and inactive icon painting can be triggered while a client is being
withdrawn. Both entry points now require valid client/screen state and a
realized icon frame before reading appearance data or issuing X drawing
requests. Normal icon repainting is unchanged.

## `mwm-icon-reparenting.md`

# MWM client icon reparenting

Client-provided icon windows can be destroyed independently of the managed
client. `ReparentIconWindow` now checks the icon and destination frame IDs and
requires `XGetGeometry` to succeed before using the returned dimensions. This
prevents stale external XIDs from causing invalid configure/reparent requests
or uninitialized placement calculations.

## `mwm-icon-resource-bounds.md`

# Mwm icon resource-key bounds

WSM saves icon positions under resource keys containing the workspace name.
Those keys are assembled in a fixed 1024-byte buffer. The builder now uses
`snprintf` and reports an unrepresentable key to the caller, which skips that
workspace entry without overwriting Mwm state.

`tests/automated/check-mwm-icon-resource-bounds.py` guards the formatting
contract.
