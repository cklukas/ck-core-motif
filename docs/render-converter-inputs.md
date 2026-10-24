# Render-table converter inputs

`XmRenderTableCvtFromProp` now rejects a missing widget or property string
before selecting the application context or invoking the lexer.  Malformed
conversion calls therefore return `NULL` instead of dereferencing invalid
input.
