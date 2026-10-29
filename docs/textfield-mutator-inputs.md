# TextField mutator input guards

The public TextField setters, replacement and insertion helpers, and add-mode
setter now return immediately for a NULL widget before asking Xt for an
application context. This preserves their void no-op behavior for invalid
widget input while preventing a precondition failure; valid text conversion,
selection, callbacks, and resource updates are unchanged.

The focused source contract is checked by
`tests/automated/check-textfield-mutator-inputs.py`.
