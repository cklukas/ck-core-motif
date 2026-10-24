# DataField destination ownership

When a DataField is asked to relinquish its destination, Motif now calls
`XtDisownSelection` only if that widget actually owns the destination
selection.  This prevents an unrelated destination owner from being disowned
and avoids clearing the display-wide destination pointer for another widget.
Normal destination acquisition and release behavior is unchanged.

The warning-enabled DataField build verifies the corrected branch structure;
the DataField Xvfb regression remains the runtime smoke check.
