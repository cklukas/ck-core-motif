---
nav_exclude: true
---
# ColorSelector optional-child updates

ColorSelector can be partially constructed when an internal slider, mode
toggle, or color list cannot be allocated.  Resource changes must remain safe
in that state: label updates now change the stored `XmString` values while
updating only children that exist, and selecting the current color becomes a
no-op when the list is unavailable.

Normal fully constructed selectors still update every child and preserve the
existing list/slider mode behavior.  The guards only make deferred resource
updates and mode changes recoverable after a partial construction failure.
