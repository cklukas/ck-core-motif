# Scrolled-list construction failure handling

`XmCreateScrolledList` creates a managed scrolled window and then creates the
list child inside it. Either Xt creation can fail (for example, after an
allocation failure). The constructor now checks both results before registering
the destroy callback. A failed window or child returns `NULL` without invoking
callbacks through an invalid widget pointer.

If the list child cannot be created after its managed scrolled window was
created, the constructor destroys that window before returning `NULL`. The
constructor owns this intermediate window until the child and its destroy
callback are installed; leaving it behind would leak an orphan managed widget.
Successful construction keeps the existing child-driven parent cleanup.
A nonzero argument count also requires a non-null argument list. Malformed
input returns `NULL` before argument merging or widget creation; the ordinary
zero-count/null-list call remains valid.

The executable regression extracts the production constructor and injects Xt
creation failures through test doubles. It verifies rollback and successful
ownership, including null names and overflowing argument counts, under ASan
and UBSan without requiring an X server or rebuilding the toolkit:

```sh
python3 tests/automated/run-scrolled-list-constructor-unit.py --out /absolute/fresh/results
```

The runner writes source fingerprints, commands and results to `receipt.json`.
This unit test covers constructor control flow and ownership; full Xt widget
lifecycle integration remains a separate test boundary.
