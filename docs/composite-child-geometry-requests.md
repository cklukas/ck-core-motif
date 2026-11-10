# Composite child geometry requests

`XmToast`, `XmCommandPalette`, and `XmSidebar` now report an accepted child
geometry request (including a child retrying the parent's proposal) as
`XtGeometryDone`, and a matching query-only request as `XtGeometryYes`.  They
continue to return `XtGeometryAlmost` only when the parent's layout requires
different dimensions.

This lets ordinary Xt children and application subclasses distinguish an
accepted request from an actual counterproposal.  The controls retain their
existing layout ownership and reject unsupported position, border, sibling, and
stacking requests.

The installed-prefix checks are exercised by:

    MOTIF_TEST_PREFIX=/path/to/prefix python3 tests/automated/run-toast.py
    MOTIF_TEST_PREFIX=/path/to/prefix python3 tests/automated/run-command-palette.py
    MOTIF_TEST_PREFIX=/path/to/prefix python3 tests/automated/run-sidebar.py
