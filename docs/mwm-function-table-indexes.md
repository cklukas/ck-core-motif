# MWM function-table indexes

MWM function-table scans now use `size_t` indexes, matching the type produced
by `sizeof(functionTable) / sizeof(functionTable[0])`. This removes signed /
unsigned comparison narrowing while preserving the existing final-entry
sentinel and function-table behavior.

The source contract is covered by
`tests/automated/check-wm-function-table-indexes.py`.
