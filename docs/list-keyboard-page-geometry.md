# List keyboard page geometry

List horizontal page navigation derives its movement from the viewport width,
character-width estimate, and border/margin insets. The old expressions mixed
the unsigned widget width with signed constants before storing the result in an
`int`; a very narrow list could wrap the page step and jump to an unrelated
horizontal position.

The four keyboard paging paths now convert the viewport width before
subtraction. Normal page movement is unchanged while collapsed lists keep
bounded navigation positions.
