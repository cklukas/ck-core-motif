# Render-table tag extraction guards

`XmRenderTableGetTags` now rejects null rendition slots and null entry tags
before copying them.  Any tags already copied are released and the output is
reset to null, so callers never receive a partial list or encounter a null
pointer dereference.
