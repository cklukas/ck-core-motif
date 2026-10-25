# FontSelector widget-list growth

FontSelector keeps the widgets that are visible or sensitive in XLFD mode in
two internal arrays. Their count and capacity fields are legacy `char` values,
so growth must remain within the representable signed range.

Both list builders now reject malformed or unrepresentable counters, check the
byte count before calling Xt, and publish the replacement pointer and capacity
only after allocation succeeds. A failed growth leaves the existing list and
its counters unchanged.
