# XmCommand API null-safety

`XmCommandGetChild`, `XmCommandSetValue`, `XmCommandAppendValue`, and
`XmCommandError` now validate the command widget before Xt app-context access.
Set, append, and error operations also reject null strings before entering the
command state machine. Valid command operations retain their existing behavior.
