---
nav_exclude: true
---
# ColorObj cache-property input validation

`ColorCachePropertyExists` now rejects missing displays or widgets, absent
selection owners, and out-of-range screen indices before calling Xlib or
forwarding property data to the pixel parser.

`tests/automated/check-color-cache-property-inputs.py` protects this contract.
