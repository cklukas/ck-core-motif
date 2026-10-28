# DataField substring conversion failures

`XmDataFieldGetSubstring` now returns `XmCOPY_FAILED` when converting a wide
substring to the active multibyte encoding fails.  It still clears the output
at the safe terminator position, but callers are no longer told that an empty
copy succeeded when the source contains an unrepresentable character.
