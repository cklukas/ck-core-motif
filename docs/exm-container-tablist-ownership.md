# Exm Container default-tab ownership

The educational Exm Container sample now owns its generated default tab list
per widget instance. Previously, a process-global tab pool and tab list meant
that asking one Exm Container for its default detail tabs could free or replace
the list returned by another instance.

The generator now builds a temporary bounded array, transfers the resulting
`XmTabList` to the owning widget only after successful construction, and frees
that list at widget destruction. Callers retain the historical contract: a
generated default list is widget-owned and must not be freed by the caller.

Run the focused source-contract check with:

```sh
python3 tests/automated/check-exm-container-tablist-ownership.py
```
