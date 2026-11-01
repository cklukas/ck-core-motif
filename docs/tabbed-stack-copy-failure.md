# Tabbed-stack label copy failure

`XmTabbedStackListCopy` now fails atomically when a non-null compound label
cannot be copied. Earlier behavior returned a list with a silently missing
label. The corrected path returns `NULL` and frees all labels copied so far,
the temporary attributes array, and the new list record. The source list is
unchanged, including labels originally marked as shared.

Successful copies remain independent deep copies with `XmTAB_VALUE_COPY`
ownership. Null labels and empty lists remain supported. This applies to the
tabbed-stack attribute list; it is separate from `XmTabList` text tab stops.

The executable regression extracts the production copy and free functions,
injects failure into all five allocations of a three-label list, checks source
preservation and zero outstanding allocations, and verifies successful deep
copies, null labels and empty lists under ASan/UBSan:

```sh
python3 tests/automated/run-tabbed-stack-copy-unit.py --out /absolute/fresh/results
```

This unit probe uses allocation and compound-string test doubles to exercise
copy ownership without rebuilding the toolkit. Source hash, commands, results
and timing are recorded in `receipt.json`; native drawing is a separate scope.
