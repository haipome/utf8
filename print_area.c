/*
 * Description: 
 *     History: damonyang@tencent.com, 2013/12/26, create
 */

# include <stdio.h>
# include <stdlib.h>
# include <inttypes.h>
# include <error.h>
# include <errno.h>

# include "utf8.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
        error(1, 0, "Usage：%s start end", argv[0]);

    int32_t start = 0, end = 0;
    sscanf(argv[1], "%"SCNx32, &start);
    sscanf(argv[2], "%"SCNx32, &end);

    int32_t i;
    for (i = start; i <= end; ++i)
    {
        char str[10] = { 0 };
        char *s = str;
        size_t left = 10;

        putu8c(i, &s, &left);
        printf("%#-6x %s\n", i, str);
    }

    return 0;
}

