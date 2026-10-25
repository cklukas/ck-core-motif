# TabList constructors

`XmTabbedStackListCreate` and `XmTabbedStackListCopy` now check their Xt
allocations before initializing or copying through the returned pointers.
Copying also rejects malformed or oversized source lists and releases the
partially created list before returning `NULL`.

The focused regression is `tests/automated/check-tablist-constructors.py`.
