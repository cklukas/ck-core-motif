# Data-field insert-selection property validation

`ConvertInsertSelection` receives an `ATOM_PAIR` property containing the
selection and target atoms.  The reader now requires the expected property
type, 32-bit format, and two items before casting the Xlib buffer to
`_XmTextInsertPair`.  Invalid or truncated selection requests are released
and rejected instead of being dereferenced as a short structure.  A buffer
returned alongside an Xlib error is released as well.
