# MWM command-tree allocation failures

`mwm` constructs the client-command tree from workspace-manager protocol
messages.  Command names and default labels are copied into tree nodes, so an
allocation failure must not be followed by a node dereference or by removal of
an existing command with the same name.

`NewCommand` now reports failure after releasing any partially constructed
node.  Both the root and ordinary command insertion paths stop cleanly when it
does so.  Replacement nodes are fully allocated before a duplicate entry and
its menu specification are removed; the removal uses the duplicate command ID,
not the incoming ID.  A missing command set retains the established top-level
fallback, while an allocation failure under an existing command set does not
silently create a different top-level command.

The focused source-contract regression is:

```
python3 tests/automated/check-mwm-command-tree-allocation.py
```

It verifies the allocation guards, the construction-before-replacement order,
and the duplicate-ID deletion path.  Normal command definition and menu
layout behavior are otherwise unchanged.
