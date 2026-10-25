# Gadget secondary-argument merging

The generic Gadget secondary-object hook merges the caller's arguments with
`XmNlogicalParent` before creating the secondary object.  The merged list is
now checked before it is passed to Xt.  If Xt cannot allocate the merged list,
the hook returns without dereferencing a null argument list; normal successful
argument merging and ownership are unchanged.

The regression check is `tests/automated/check-gadget-secondary-args.py`.
