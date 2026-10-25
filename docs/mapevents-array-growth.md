# MapEvents parallel-array growth

Event translation parsing returns three parallel arrays: event types,
keysyms, and modifiers.  The old parser resized them one at a time.  If a
later resize failed, the output arrays no longer described the same number of
events.

Each parsed event now grows all three arrays together, copies the existing
prefix, and commits the replacement set only after every allocation succeeds.
The parser reports failure with the previous prefix still consistent.
