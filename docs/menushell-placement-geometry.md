# MenuShell placement geometry

Menu placement computes the right and bottom edges by adding a signed screen
position to unsigned border and widget dimensions.  On a large or temporarily
out-of-range menu, the old expression could wrap before the on-screen checks,
causing incorrect repositioning.

The edge calculations now use signed integer intermediates. Existing clamping
keeps the final menu origin on screen while making oversized-menu placement
deterministic.
