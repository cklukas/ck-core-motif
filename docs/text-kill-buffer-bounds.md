# Text kill-buffer bounds

The `DeleteOrKill` action now validates the temporary string used for a kill
operation before handing it to Xlib. If the source cannot produce a snapshot,
or if its byte length cannot be represented by Xlib's `int`-sized cut-buffer
interface, the action rings the existing error bell and leaves the document
unchanged. Successful snapshots are converted only after the checked bound.

The focused source contract is checked by
`tests/automated/check-text-kill-buffer-bounds.py`.
