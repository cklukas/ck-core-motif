---
nav_exclude: true
---

# Validate XPM data-export arguments

`XpmCreateDataFromXpmImage` now rejects a null output-array pointer or source
image before writing through the output or reading image metadata.  Optional
`XpmInfo` metadata remains supported exactly as before.
