# Scale child-count bounds

`XmScale` keeps its child count in the Xt `Cardinal` type. Label measurement
and horizontal/vertical label layout now use `Cardinal` indices when walking
the composite child array, including the right-to-left layout path. This
preserves the existing child ordering and placement behavior while avoiding
signed-index wraparound for large child lists.
