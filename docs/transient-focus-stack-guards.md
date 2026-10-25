# Transient focus and stacking guards

Focus repair and transient-tree normalization can run after a client has been
removed.  `FindTransientFocus` now falls back to the current keyboard focus,
while `BumpPrimaryToBottom` and `NormalizeTransientTreeStacking` report no
change for a missing tree.  Valid modal focus and stacking behavior is
unchanged.
