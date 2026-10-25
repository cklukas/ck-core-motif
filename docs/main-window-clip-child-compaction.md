---
nav_exclude: true
---
# MainWindow clip-child compaction

When automatic scrolling discovers a child in the clip window that belongs
to the main window, `XmMainWindow` moves it to the main-window child list and
removes it from the clip list. The compaction loop now writes each trailing
sibling to `j - 1` and revisits the same slot after removal. This prevents
duplicate or skipped children when adjacent clip-window entries are moved.
The related arithmetic regression check accepts the maintained declaration's
additional local width variable instead of depending on an obsolete exact
declaration prefix.
