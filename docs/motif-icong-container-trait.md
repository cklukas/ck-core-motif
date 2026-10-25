---
nav_exclude: true
---
# IconGadget container-trait validation

IconGadget now checks that its container-parent hook returns a widget and that
the container trait provides `getValues` before using it. Color lookup keeps
its existing fallback colors when the optional trait is absent or partial.
This protects custom container implementations without changing the trait ABI.
