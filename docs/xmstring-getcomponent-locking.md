# XmStringGetNextComponent locking

`XmStringGetNextComponent` now delegates synchronization to
`XmeStringGetComponent` instead of holding the process lock across that call.
This avoids recursive locking while keeping component extraction serialized by
the existing helper.

The source contract is covered by
`tests/automated/check-xmstring-getcomponent-locking.py`.
