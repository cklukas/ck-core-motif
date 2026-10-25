# XmString proposed tab-list allocation failure

`XmStringTableProposeTablist` now checks creation of its initial tab and tab
list before use.  Failure releases any temporary tab and the process lock
without entering the measurement loop.

The regression check is
`tests/automated/check-xmstring-tablist-create.py`.
