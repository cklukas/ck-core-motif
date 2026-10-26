# Text event hit-test bounds

Text actions that convert button-event coordinates into text positions now
saturate both axes before calling the Position-based `XYToPos` callback. This
covers cursor placement, primary/secondary selection, in-selection checks, and
destination movement, preventing large translated coordinates from narrowing
or wrapping inconsistently between actions.

The focused source contract is checked by
`tests/automated/check-text-event-hit-test-bounds.py`.
