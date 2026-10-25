---
nav_exclude: true
---
# ScrollFrame navigator-list growth

ScrollFrame navigator registration grows its widget list in two-slot steps.
The reallocation is now transactional: the slot count and pointer are updated
only after `XtRealloc` succeeds.  If growth fails, registration returns while
the existing navigator list remains valid.

The regression check is
`tests/automated/check-scrollframe-nav-growth.py`.
