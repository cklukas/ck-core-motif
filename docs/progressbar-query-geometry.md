---
nav_exclude: true
---
# ProgressBar geometry negotiation

`XmProgressBar` computes preferred dimensions from its label, style, and
spacing.  Those dimensions are a fallback for an unallocated widget; an
explicit application allocation is now retained by `QueryGeometry` and
negotiated through `XmeReplyToQueryGeometry`.

This avoids a parent geometry query silently reverting a deliberately narrow,
wide, short, or tall progress indicator to its content-derived default.

The modern percentage-text overlay also clamps its filled clip segment to the
actual progress track and converts clip coordinates through the toolkit's
bounded X11 `Position` helper.  Large but representable widget allocations
therefore cannot wrap the signed clip origin or extend its unfilled segment.
