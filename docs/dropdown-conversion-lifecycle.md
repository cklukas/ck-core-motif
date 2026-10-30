---
nav_exclude: true
---
# DropDown conversion lifecycle

Popup placement and text/list conversion now validate the required internal
children before querying or updating them.  A partially constructed DropDown
returns conversion failure or an empty value safely; popup placement no longer
reads missing text or arrow geometry.

Fully constructed controls retain their existing selection, text conversion,
and popup behavior.
