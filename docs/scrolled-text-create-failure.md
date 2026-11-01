# Scrolled-text construction failure handling

`XmCreateScrolledText` checks its temporary name and argument allocations,
the created scrolled window, and the text child before using them. If child
creation fails, it destroys the constructor-owned scrolled window. Each
failure returns `NULL` after freeing temporary storage and balancing the
application lock. A nonzero argument count requires a non-null argument list.

Successful creation retains the established text-child destroy callback that
owns parent cleanup. Public signatures and widget resources are unchanged.

The executable unit regression extracts this production constructor and
injects allocation and widget-creation failures through Xt test doubles. It
checks cleanup, balanced locking, successful construction, null names and
argument bounds under ASan/UBSan:

```sh
python3 tests/automated/run-scrolled-list-constructor-unit.py --kind text --out /absolute/fresh/results
```

`receipt.json` records the tested source fingerprint, commands and results.
This is a constructor ownership unit test; it does not substitute for native
Xt widget-lifecycle integration.
