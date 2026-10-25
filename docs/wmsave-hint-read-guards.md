# WM save-hint property validation

`ProcessWmSaveHint` reads the first 32-bit value from `_MOTIF_WM_HINTS`'s
save-hint property.  X permits a property to have the expected type and
format while containing no items.  The reader now requires at least one item
before dereferencing the returned buffer, and continues to clear the saved
flags when the property is absent or malformed.

This preserves the existing behavior for valid clients while preventing a
malformed empty property from causing a null-pointer dereference in `mwm`.
