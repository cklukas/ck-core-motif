---
nav_exclude: true
---
# Render-table tag arrays

`XmRenderTableGetTags` now validates its output pointer and handles every
allocation failure. A failed tag allocation releases any tags already copied,
clears the caller's output, and returns zero. The successful result still
contains one independently allocated tag for every rendition and retains the
documented `XtFree` ownership rules. Display-less render tables use the
process lock instead of attempting to derive an application context from a
null display.
