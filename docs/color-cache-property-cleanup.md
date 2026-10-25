---
nav_exclude: true
---
# Color-cache property cleanup

`ColorCachePropertyExists` reads the private pixel-set property from the
selection owner.  Xlib may return an allocated buffer even when the request
fails or the property has an unusable format.  The failure path now releases
that buffer before returning, preventing a leak while retaining the existing
`False` result for malformed cache properties.
