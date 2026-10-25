# Removing a font-list entry with no tag

`XmFontListRemoveEntry` now returns the original list when the supplied
rendition has no tag.  It avoids copying a null tag and leaves the existing
font list unchanged, matching the defensive behavior of the other font-list
tag helpers.
