# Container tab-list ownership

Dynamic Container detail tabs are now created in per-call storage. The
temporary `XmTab` values are released after `XmTabListInsertTabs` copies them,
so separate Containers no longer share a process-global pool or retain stale
capacity. Valid narrow layouts keep their tab list instead of silently
disabling detail columns at a fixed 30-pixel cutoff. The public resource and
tab-list behavior is unchanged. If an individual tab allocation fails, already
created tabs are released and the Container returns no tab list instead of
calling `XmTabSetValue` through a null pointer.

Run the general native Container regression against an installed prefix with:

```sh
python3 tests/automated/run-container-regressions.py /absolute/path/to/prefix /absolute/path/to/results
```

The dynamic tab-pool builder also has an executable unit regression:

```sh
python3 tests/automated/run-container-tab-pool-unit.py --out /absolute/fresh/results
```

It extracts `GetDumbTabList` directly from `Container.c` and uses allocation
and tab-list test doubles. Failure is injected into all ten allocations of a
seven-column result; every failure must release all temporary ownership.
Successful cases check every position for 1, 257 and 65,537 columns with
`INT_MAX` spacing against a double-precision arithmetic oracle, and verify
that generating another list leaves a retained list unchanged. Empty and
unrepresentable requests must allocate nothing. The runner compiles with
strict warnings and ASan/UBSan and writes source hashes, commands, results
and timing to `receipt.json`.

This checks the builder's ownership and arithmetic without rebuilding the
toolkit; it does not qualify widget rendering or multiple X11 displays.
