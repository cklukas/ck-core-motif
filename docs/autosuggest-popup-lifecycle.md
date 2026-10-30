---
nav_exclude: true
---
# AutoSuggestBox popup lifecycle

AutoSuggestBox now checks popup-shell creation before constructing its list
child.  A failed shell allocation returns cleanly instead of passing a null
parent to `XmCreateList`.

The transient shell also clears its shell and list pointers when destroyed
externally, allowing the next suggestion request to create a fresh popup.
