# TextField clipboard selection restart guard

After a clipboard replacement, TextField restarts the primary selection only
when the resulting primary range is nonempty and the destination is eligible.
The restart and its process lock are now enclosed in the same conditional
block. Previously, missing braces made the restart call unconditional even
when the range was empty, and the compiler's misleading-indentation warning
masked that control-flow error.

The source contract is covered by
`tests/automated/check-textfield-selection-restart-guard.py`.
