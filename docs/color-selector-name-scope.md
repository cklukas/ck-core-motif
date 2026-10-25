# Color selector replacement-name scope

`XmColorSelector` can replace a hexadecimal color name with the matching
named color after parsing the slider value. The replacement pointer now has
function scope, so both selector update paths compile while preserving the
existing allocation, failure, and ownership behavior.
