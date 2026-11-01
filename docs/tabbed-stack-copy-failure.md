# Tabbed-stack label ownership and allocation failure

`XmTabbedStackListCopy` now fails atomically when a non-null compound label
cannot be copied. Earlier behavior returned a list with a silently missing
label. The corrected path returns `NULL` and frees all labels copied so far,
the temporary attributes array, and the new list record. The source list is
unchanged, including labels originally marked as shared.

Successful copies remain independent deep copies with `XmTAB_VALUE_COPY`
ownership. Null labels and empty lists remain supported. This applies to the
tabbed-stack attribute list; it is separate from `XmTabList` text tab stops.

`XmTabbedStackListInsert` (also used by `XmTabbedStackListAppend`) prepares a
copied label before modifying the list. If label allocation or array growth
fails, it returns `XmTAB_NOT_FOUND` with the original entries, count, capacity
and storage unchanged. A prepared label is freed if subsequent growth fails.
Incoming attributes are snapshotted before shifting or reallocating storage,
so an existing entry can safely provide the attributes for a new entry.
Shared-label ownership and successful insertion behavior remain unchanged;
no public API or structure layout changes are required.

`XmTabbedStackListModify` honors `XmTAB_LABEL_STRING` when switching from
shared to copied ownership. An ownership-only update copies the current label;
it does not read the caller's unselected label field or silently replace the
label with that value. Selecting both flags copies the supplied replacement.
Allocation failure leaves ownership, label and other selected attributes
unchanged. Null labels remain valid, and the existing restriction against
switching copied labels back to shared ownership is preserved.

The executable regression extracts the production copy, insert, modify and free functions,
injects failure into all five allocations of a three-label list, checks source
preservation and zero outstanding allocations, and verifies successful deep
copies, null labels and empty lists under ASan/UBSan. Insertion cases inject
label-copy and array-growth failures with and without spare capacity, use
aliased attributes across reallocation and shifting, and check shared labels,
appending and default attributes. Modification cases verify mode-only label
preservation, independent copies, explicit replacement, null labels and
allocation-failure rollback:

```sh
python3 tests/automated/run-tabbed-stack-copy-unit.py --out /absolute/fresh/results
```

This unit probe uses allocation and compound-string test doubles to exercise
copy ownership without rebuilding the toolkit. Source hash, commands, results
and timing are recorded in `receipt.json`; native drawing is a separate scope.
