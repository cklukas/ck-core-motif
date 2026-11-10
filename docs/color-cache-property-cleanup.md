---
nav_exclude: true
---
# Color-cache property cleanup

`ColorCachePropertyExists` reads the private pixel-set property from the
selection owner.  Xlib may return an allocated buffer even when the request
fails or the property has an unusable format.  The failure path now releases
that buffer before returning, preventing a leak while retaining the existing
`False` result for malformed cache properties. A buffer that
parses is handed to `FetchPixelData`, which copies it into a private bounded
buffer and releases the Xlib buffer on every path; the caller must not release
it again. `run-color-object-property-ownership.py` publishes the property of a
color server and creates a Motif shell under AddressSanitizer.
