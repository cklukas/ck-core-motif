# Label baseline allocation

`XmLabelGetBaselines` caches an extra baseline offset and returns a
caller-owned copy. Cache growth now uses a temporary pointer with checked
size arithmetic, and the returned copy is checked before it is filled.

Missing or unrepresentable baseline data returns `False`; an empty baseline
set returns success with a null copy. Successful baseline values and offsets
remain unchanged.
