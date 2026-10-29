# DataField selection-action guards

The public DataField remove, copy, cut, and paste actions now reject a NULL
widget before Xt context, display, or window lookup. They retain their existing
`False` failure result without touching clipboard state; valid selection,
clipboard, conversion, and callback behavior is unchanged.

The focused source contract is checked by
`tests/automated/check-datafield-selection-actions.py`.
