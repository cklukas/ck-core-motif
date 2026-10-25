# XmRenderTable resource-cache allocation

`XmRenderTable` caches quarkized resource names and the per-call `found`
flags while resolving rendition resources.  Initial allocation failures now
return cleanly, argument-driven quark-cache growth checks both host `size_t`
and Xt `Cardinal` byte limits before committing a replacement, and Xrm
search-list growth rejects overflow or allocation failure through the existing
cleanup path.

The focused regression is
`tests/automated/check-xmrender-resource-cache-allocation.py`.
