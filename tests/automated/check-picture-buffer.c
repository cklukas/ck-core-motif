/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Picture.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    XmPicture picture = XmParsePicture("*@");
    XmPicture malformed = XmParsePicture("*999999999999999999999x");
    XmPictureState state;
    Boolean finished = False;
    char input[2049];
    char *current;

    if (!picture) {
        fputs("picture parse failed\n", stderr);
        return 1;
    }
    if (!malformed) {
        fputs("overflowing repeat count rejected the picture\n", stderr);
        return 1;
    }
    XmPictureDelete(malformed);
    memset(input, 'x', sizeof(input) - 1);
    input[sizeof(input) - 1] = '\0';
    state = XmGetNewPictureState(picture);
    for (size_t i = 0; i < sizeof(input) - 1; ++i) {
        if (!XmPictureProcessCharacter(state, input[i], &finished)) {
            fputs("picture stopped before the long input completed\n", stderr);
            return 1;
        }
    }
    current = XmPictureGetCurrentString(state);
    if (!current || strlen(current) != sizeof(input) - 1) {
        fprintf(stderr, "long picture length was %zu\n", current ? strlen(current) : 0UL);
        return 1;
    }
    XmPictureDeleteState(state);
    XmPictureDelete(picture);
    puts("PASS XmPicture dynamic current-string growth");
    return 0;
}
