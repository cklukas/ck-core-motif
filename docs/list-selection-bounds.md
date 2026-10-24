# List selection result buffers

`XmListGetMatchPos` and `XmListGetSelectedPos` now check the number of result
integers against `size_t` before allocating and copying the caller-visible
arrays. Ordinary empty and selected-list behavior is unchanged.
