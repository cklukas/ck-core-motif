# MWM dialog context lookups

Dialog lifecycle callbacks recover their client record from an X context. Both
withdraw and remanage paths now require the lookup to return a non-null
`ClientData` pointer before changing lists, focus, or frame state.
