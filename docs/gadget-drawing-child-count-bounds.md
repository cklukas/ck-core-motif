# Gadget and DrawingArea child-count bounds

Gadget redisplay and DrawingArea hit testing now use Xt `Cardinal` indices for
composite child arrays. DrawingArea's reverse hit-test loop explicitly checks
the count before decrementing, so an empty child list cannot underflow while
retaining topmost-child hit-test ordering.
