# MenuShell empty-child guards

MenuShell popdown helpers historically assumed that every shell had a first
child. During private-shell teardown or an interrupted menu construction, an
empty shell can reach those helpers and dereference `children[0]`.

Fast popdown now unmaps an empty shell, normal popdown safely closes it when
needed, and recursive child traversal returns immediately. Normal one-child
menu behavior is unchanged. The regression
`tests/automated/check-menushell-empty-guards.py` covers all three paths.
