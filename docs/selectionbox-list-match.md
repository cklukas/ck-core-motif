# SelectionBox list match positions

SelectionBox creation can select an initial text match by asking the child
List for matching positions.  A positive match count does not guarantee that
the returned position array is available after an allocation failure.  The
initial-selection path now requires both values before reading `position[0]`;
normal initial text matching remains unchanged.

The regression contract is checked by
`tests/automated/check-selectionbox-list-match.py`.
