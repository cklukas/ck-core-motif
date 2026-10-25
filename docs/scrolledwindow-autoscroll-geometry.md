# ScrolledWindow auto-scroll rectangles

Automatic scrolling builds edge rectangles from the scrolled-window size and
the translated work-area origin. The old width and height subtractions mixed
unsigned dimensions with signed rectangle positions, so a work area beyond a
collapsed window could wrap into a huge auto-scroll region.

The horizontal and vertical edge spans now use signed intermediates and retain
the existing minimum span of two pixels. Normal auto-scroll behavior is
unchanged while collapsed windows remain bounded.
