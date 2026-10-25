# Adding render tables with null entries

`XmRenderTableAddRenditions` now rejects a rendition array containing a null
entry before reading its display or modifying a table.  Existing tables are
returned unchanged, while valid arrays retain their normal merge and copy
behavior.
