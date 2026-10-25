# Tooltip trait allocation

`XmSetToolTipString` now checks the allocation of a missing tooltip trait
before registering or using it. If allocation fails, the setter safely leaves
the existing widget state unchanged; successful tooltip creation and updates
are unchanged.
