# TabList argument guards

`XmTabbedStackListInsert` and `XmTabbedStackListModify` now reject a `NULL`
attribute pointer when the caller supplies a nonzero attribute mask. The
existing list is left unchanged and insertion returns `XmTAB_NOT_FOUND`; valid
zero-mask operations remain compatible.

The focused regression is `tests/automated/check-tablist-argument-guards.py`.
