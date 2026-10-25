# ColorObj initialization cleanup

ColorObj initialization publishes the per-display object before allocating
its per-screen arrays. If one of those allocations fails, the object is now
removed from both the X context and the global ColorObj chain before the
initializer returns. This prevents later lookups from reusing an unusable
partially initialized object.
