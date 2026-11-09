# MWM screen-record allocation

MWM obtains its managed screen count from Xlib and configuration processing,
then stores one `WmScreenData` record for each screen.  Before using the count
for the Xt `Cardinal` allocator, startup now requires a positive count and
checks the record-array byte size against both host `size_t` and Xt's limit.
Invalid counts stop initialization before backdrop or screen access; valid
single- and multi-screen behavior is unchanged.
