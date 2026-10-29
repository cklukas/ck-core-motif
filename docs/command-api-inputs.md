---
nav_exclude: true
---
# Command convenience API inputs

The public Command helpers now reject non-Command widget handles before
obtaining an application context or reading Command-private children.  This
covers `XmCommandGetChild`, `XmCommandSetValue`, `XmCommandAppendValue`, and
`XmCommandError`; null and optional-value behavior remains unchanged.

The focused source regression is
`tests/automated/check-command-api-inputs.py`.
