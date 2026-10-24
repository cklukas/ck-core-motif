# Container tab-list ownership

Dynamic Container detail tabs are now created in per-call storage. The
temporary `XmTab` values are released after `XmTabListInsertTabs` copies them,
so separate Containers no longer share a process-global pool or retain stale
capacity. Valid narrow layouts keep their tab list instead of silently
disabling detail columns at a fixed 30-pixel cutoff. The public resource and
tab-list behavior is unchanged.

Run the native regression against an installed prefix with:

```sh
python3 tests/automated/run-container-regressions.py /absolute/path/to/prefix /absolute/path/to/results
```
