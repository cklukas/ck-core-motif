# CommandPalette selected-position result

CommandPalette invocation maps the selected List position into its filtered
visible-item array.  A positive selection count does not guarantee that the
position array is available after an allocation failure.  Invocation now
rejects a null array before reading `positions[0]`; valid selections and
callback delivery remain unchanged.

The regression contract is checked by
`tests/automated/check-command-palette-selection-pointer.py`.
