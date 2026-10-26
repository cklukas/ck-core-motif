# Secondary selection extension

Secondary-selection extension now checks the Boolean result from
`_XmTextGetSel2` before using its output positions.  If no secondary selection
exists, the operation restores redisplay and returns; it no longer compares or
passes uninitialized positions to the source scanner and selection setter.

The focused source contract is checked by
`tests/automated/check-text-secondary-extend-selection.py`.
