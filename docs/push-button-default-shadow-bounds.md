# PushButton default shadow bounds

`XmPushButton` chooses either the external highlight thickness or the internal
3D enhancement as the default-button shadow delta.  Its previous bounds check
allowed the internal delta to exceed a small widget before subtracting it from
`Dimension` values.

The draw path now rejects dimensions no larger than the selected delta before
computing the inner rectangle.  Valid default-button shadows keep their
existing emphasis and colors.
