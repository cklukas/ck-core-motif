---
nav_exclude: true
---

# Image-scaling helper outputs

The image-cache scaling helpers now reject null output pointers before writing
rounded dimensions.  This keeps malformed or future direct helper calls from
turning otherwise bounded scaling input into a null-pointer write.

The regression check is
`tests/automated/check-image-scale-output-pointers.py`.
