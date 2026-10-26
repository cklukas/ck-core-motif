# MWM frame display-list counts

When title and resize appearance are shared, `AllocateFrameDisplayLists`
allocates one top and one bottom shadow list containing both kinds of frame
elements. The bottom-list count now combines `NUM_RESIZE_BS_ELEMENTS` with
`NUM_TITLE_BS_ELEMENTS`; the previous code added the resize count twice and
could size the list incorrectly for themes with different title and resize
geometry.

The separate-title path remains unchanged. The source contract is covered by
`tests/automated/check-mwm-frame-display-list-counts.py`.
