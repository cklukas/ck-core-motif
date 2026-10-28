# TextField selection-state naming

The destination-selection transfer state is process-global because the legacy
transfer protocol passes it through callback closures. It is now named
`insert_select_state`, distinct from each callback's local closure pointer.
This removes `-Wshadow` ambiguity without changing the closure layout or
public transfer behavior.

The analogous primary-selection state is named `primary_select_state` for the
same reason.

The source contract is covered by
`tests/automated/check-textfield-selection-state-naming.py`.
