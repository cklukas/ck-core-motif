---
nav_exclude: true
---

# AutoSuggestBox popup dimensions

AutoSuggestBox derives its popup width and height as signed working values so
it can perform edge and available-space calculations.  The dimensions are
bounded by the screen's signed coordinate domain before they cross the Xt
`Dimension` boundary, and the final conversion is explicit when configuring
the scroller.  This prevents signed-to-unsigned narrowing from making a popup
unexpectedly huge.

`tests/automated/check-autosuggest-popup-dimensions.py` guards that boundary.
