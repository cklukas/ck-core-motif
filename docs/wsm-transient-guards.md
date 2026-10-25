# WSM transient helper guards

WSM transient stacking can run while workspace membership is being detached.
`FindSubLeaderToTop`, `MakeTransientFamilyStackingList`, and `LeaderOnTop`
now reject missing tree nodes or output buffers, returning neutral results
instead of dereferencing stale state.  Valid WSM stacking behavior is
unchanged.
