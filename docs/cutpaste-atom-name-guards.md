# CutPaste atom-name guards

Clipboard conversion now handles failed `XGetAtomName` calls in both target
format lookup and returned-type inspection.  A failed target lookup reports a
conversion failure; a failed returned-type lookup preserves the atom as the
type without indexing a null string.  Successful clipboard behavior is
unchanged.
