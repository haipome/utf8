# include <stdio.h>
# include <ctype.h>
# include <string.h>
# include <stdlib.h>
# include "utf8.h"

uint32_t hex2int(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else {
        return c - 'a' + 10;
    }
}

char *parse(const char *input)
{
    size_t input_size = strlen(input);
    char *output = malloc(input_size + 1);
    size_t inpos = 0;
    size_t outpos = 0;

    while (input[inpos] != 0) {
        if (input_size - inpos >= 6 && input[inpos] == '\\' && input[inpos + 1] == 'u' && isxdigit(input[inpos + 2]) && \
                isxdigit(input[inpos + 3]) && isxdigit(input[inpos + 4]) && isxdigit(input[inpos + 5])) {
            ucs4_t uc = (hex2int(input[inpos + 2]) << 12) + (hex2int(input[inpos + 3]) << 8) + (hex2int(input[inpos + 4]) << 4) + (hex2int(input[inpos + 5]));
            ucs4_t us[2];
            us[0] = uc;
            us[1] = 0;
            outpos += u8encode(us, output + outpos, 4, NULL);
            inpos += 6;
        } else {
            output[outpos++] = input[inpos++];
        }
    }

    output[outpos] = 0;
    return output;
}

int main()
{
    char *line = NULL;
    size_t buf_size = 0;

    while (getline(&line, &buf_size, stdin) != -1) {
        char *output = parse(line);
        printf("%s", output);
        free(output);
    }

    return 0;
}

