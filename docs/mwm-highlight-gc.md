# MWM highlight graphics-context helper

`GetHighlightGC` obtains bevel graphics contexts through the screen's top-level
widget. It now returns `NULL` when that screen/widget state is unavailable,
avoiding a null dereference during early initialization or teardown.
