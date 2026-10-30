# Chip label destruction

`XmChip` copies its label `XmString` during initialization and replacement.
The widget now releases that owned copy in `Destroy`, preventing label storage
from accumulating across repeated chip creation and destruction.
