# ToggleButton gadget input guards

The direct ToggleButton gadget state getter and setters now reject a NULL
widget before Xt application-context lookup. They retain the established
empty/no-op results (`False` for state/value operations and no action for the
void setter), while valid gadget state, drawing, and callback behavior is
unchanged.

The focused source contract is checked by
`tests/automated/check-toggle-gadget-inputs.py`.
