# ComboBox Up-navigation boundary

ComboBox list positions are one-based.  The Up action previously accepted the
first position (`1`) and subtracted one before calling `XmListSelectPos`,
passing the invalid position `0`.  Up now moves only when the selected
position is greater than one; at the first item it remains selected, matching
normal keyboard navigation and preserving the public one-based List contract.

The regression contract is checked by
`tests/automated/check-combobox-up-boundary.py`.
