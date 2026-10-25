# Geometry and tracking child-count bounds

The shared `GMUtils` geometry helpers and `TrackLoc` input hit-test helper now
use Xt `Cardinal` indices for manager composite child arrays. Geometry,
overlap, margin, and hit-test behavior are unchanged while large child lists
avoid signed-index wraparound.
