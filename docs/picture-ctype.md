# Picture matcher character classification

Picture matching classifies and uppercases input bytes with the C ctype
functions.  Every such call now receives an `unsigned char` value, so bytes
outside ASCII no longer invoke undefined behavior when a picture is matched.
