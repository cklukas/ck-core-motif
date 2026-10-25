# Font-list constructor failures

`XmFontListCreate` and `XmFontListCreate_r` now propagate a failed rendition
allocation immediately.  They no longer pass a null rendition into render
table construction, which keeps allocation failures deterministic and avoids
dereferencing incomplete entries.
