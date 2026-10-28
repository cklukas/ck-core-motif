# Text selection-state naming

The legacy Text selection-transfer callbacks share process state through
closure pointers. The shared records are now named `insert_select_state` and
`primary_select_state`, distinct from callback-local closure variables. This
removes `-Wshadow` ambiguity without changing record layout, closure values, or
the public selection-transfer contract.

The source contract is covered by
`tests/automated/check-text-selection-state-naming.py`.
