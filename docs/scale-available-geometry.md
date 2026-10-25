# Scale available trough geometry

Scale computes the trough area after subtracting leading and trailing
padding. Horizontal and vertical layout now perform those subtractions in a
signed `int` before comparing with the scrollbar minimum and converting back to
`Dimension`. Small controls therefore take the existing minimum-size path
instead of wrapping to a huge trough dimension.
