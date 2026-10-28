---
nav_exclude: true
---
# Drag motion-buffer bounds

The drag motion buffer begins with 120 entries embedded in the stack frame.
When the boundary is reached, the heap replacement now reserves one additional
entry before recording the next motion event.  Later growth retains the existing
chunked capacity policy, and the valid motion sequence is unchanged.
