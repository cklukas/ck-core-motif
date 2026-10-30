# Scrolled-list construction failure handling

`XmCreateScrolledList` creates a managed scrolled window and then creates the
list child inside it. Either Xt creation can fail (for example, after an
allocation failure). The constructor now checks both results before registering
the destroy callback. A failed window or child returns `NULL` without invoking
callbacks through an invalid widget pointer.

This preserves the existing successful construction and cleanup behavior while
making allocation failure deterministic for embedders. The source-contract
regression is `tests/automated/check-scrolled-list-create-failure.py`.
