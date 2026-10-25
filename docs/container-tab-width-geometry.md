---
nav_exclude: true
---
# Container dynamic tab width geometry

Dynamic detail tabs derive their available width by subtracting twice the
container margin and the first-column width from the unsigned widget width.
The old expression performed those subtractions as unsigned arithmetic before
assigning to an `int`; a narrow container could therefore appear to have a
large positive tab width and allocate an unusable tab list.

The calculation now converts each dimension to signed arithmetic before
subtraction. The existing non-positive checks then discard the tab list when
the container cannot provide usable detail space.
