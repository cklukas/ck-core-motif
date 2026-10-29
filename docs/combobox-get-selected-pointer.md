# ComboBox selected-position export

The synthetic `XmNselectedPosition` export obtains both a count and an array
from the child List.  A positive count does not make the array pointer safe to
dereference if the query cannot provide it.  The export now returns the
documented no-selection value (`0`) unless both the count and pointer are
valid; ordinary selected positions and zero-based conversion are unchanged.
The export also returns safely when Xt supplies no widget or output pointer.

The regression contract is checked by
`tests/automated/check-combobox-get-selected-pointer.py`.
