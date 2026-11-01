# List sensitivity cancellation

A native List cancels a pending selection gesture when it or an ancestor loses
effective sensitivity. A later release after re-enabling requires a new press;
it cannot complete the cancelled gesture. Pending drag initiation and automatic
scroll timers are removed, and append-selection state is cleared. Existing
selection data remains intact.

`run-list-sensitivity.py` exercises single, multiple, browse and extended
selection with direct and ancestor sensitivity changes in classic, modern light
and modern dark appearances. It verifies that cancellation suppresses the old
release while a subsequent fresh gesture still notifies normally.
