# List callback item-copy failure

List selection and default-action callbacks now require a successful private
copy of the primary item string.  If allocation fails, the interaction is
cleanly abandoned and its down-count is reset rather than dispatching a
callback with a NULL item or borrowing storage that may be changed by the
callback.

The focused regression is
`tests/automated/check-list-selection-callback-allocation.py`.
