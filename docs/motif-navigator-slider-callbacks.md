---
nav_exclude: true
---
# Navigator slider callback validation

List and ScrolledW slider callbacks now validate the navigator trait and its
`getValue` callback before reading the current position. A partial or missing
trait is ignored safely, preserving normal behavior for complete navigator
implementations and avoiding a null function-pointer call during scrolling.
