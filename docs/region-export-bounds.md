# Region rectangle export bounds

The region-to-`XRectangle` export path feeds both an Xt allocation-size
(`Cardinal`) and Xlib's signed `int` rectangle count.  The old code narrowed
both without checking the region count, allowing a sufficiently large region
to wrap the allocation or the Xlib argument.

The exporter now returns no rectangle array for counts that cannot fit either
interface.  Callers already treat a null array with a nonzero count as an
allocation failure, so ordinary exports and clipping behavior remain
unchanged.  The source regression in
`tests/automated/check-region-export-bounds.py` protects both checks.
