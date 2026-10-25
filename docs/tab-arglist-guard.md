# Tab creation argument validation

The internal `_XmCreateTab` adapter now rejects a nonzero argument count with
no `ArgList` before iterating the arguments. Valid tab creation and all
existing resource conversions retain their behavior.
