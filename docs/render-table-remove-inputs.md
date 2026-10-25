# Render-table removal inputs

`XmRenderTableRemoveRenditions` now rejects a null table, null tag array,
non-positive count, or null tag entry before deriving an application context.
The internal helper repeats the count and tag validation for callers that do
not use the public wrapper.  Valid removal requests retain their existing
copy-on-write and matching behavior.
