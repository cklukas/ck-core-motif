# DataField picture verification lifetime

Picture verification now releases the temporary current-value string on both
accepted and rejected edits.  Rejected edits also release the splice buffer
and picture state before returning.  This keeps repeated picture edits from
leaking widget-owned conversion state while preserving validation callbacks,
autofill behavior, and the public DataField interface.

The warning-enabled DataField build and the DataField action regression are
the native smoke checks for this path.
