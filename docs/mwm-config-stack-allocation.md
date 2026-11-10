# MWM configuration-stack allocation handling

MWM initializes and extends its configuration-file stack transactionally. If
allocation of either a stack entry, its owned file-name copy, or a parser buffer
fails, no partial global stack is published and the parent parser remains open.
The configuration-file opener closes its provisional file and returns failure
rather than allowing the optional preprocessor path to dereference a null stack
entry. Expanded include filenames remain owned by the include parser and are
released whether pushing the child frame succeeds or fails.

The focused source-contract regression check is
`tests/automated/check-mwm-config-stack-allocation.py`.
