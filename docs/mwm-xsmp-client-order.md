# MWM XSMP client ordering

Session-managed clients are sorted first by screen and, with workspace
support, by their current workspace ID.  The comparator now uses relational
comparisons and returns `-1`, `0`, or `1`; it no longer subtracts identifiers
into a signed result.  Large or otherwise unbalanced IDs therefore cannot
overflow the comparator or make `qsort` observe an inconsistent ordering.
