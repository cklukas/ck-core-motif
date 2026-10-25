# List hit-test geometry

List hit testing compares event Y coordinates with the bottom of the content
area, derived by subtracting the signed `BaseY` origin from the unsigned widget
height. When a list is temporarily shorter than its insets, the old expression
wrapped before comparison and could select an item outside the visible area.

The comparisons now perform signed subtraction, preserving normal hit testing
while keeping collapsed-list selection bounded.
