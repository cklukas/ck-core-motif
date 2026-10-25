# SelectionBox child-count bounds

SelectionBox geometry setup now traverses its composite child array with an
Xt `Cardinal` index, including the reverse traversal used for right-to-left
button layout. Existing widget ordering and geometry behavior are unchanged.
