# Mrm create-close handling

The write-open path now checks the `close` result for the descriptor returned by
`creat`. If closing the newly created file fails, Mrm unlinks the partial file
and returns failure instead of proceeding with an ambiguous descriptor state.
