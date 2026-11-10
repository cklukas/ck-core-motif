# MWM configuration-stack allocation handling

MWM now initializes its configuration-file stack transactionally. If allocation
of either the stack entry or its owned file-name copy fails, no partial global
stack is published. The configuration-file opener closes its provisional file
and returns failure rather than allowing the optional preprocessor path to
dereference a null stack entry.

The focused source-contract regression check is
`tests/automated/check-mwm-config-stack-allocation.py`.
