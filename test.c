/*
 * Description: 
 *     History: damonyang@tencent.com, 2013/11/22, create
 */

# undef  _GNU_SOURCE
# define _GNU_SOURCE

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <error.h>
# include <assert.h>

# include "utf8.h"

int main()
{
    char *line = NULL;
    size_t buf_size = 0;

    while (getline(&line, &buf_size, stdin) != -1)
    {
        size_t len = strlen(line);
        if (len && line[len - 1] == '\n')
            line[--len] = 0;

        printf("in len: %zu\n", len);

        size_t max = len + 1;
        ucs4_t *us = calloc(max, sizeof(ucs4_t));
        int illegal = 0;

        size_t n = u8decode(line, us, max, &illegal);
        printf("unicode num: %zu, illegal: %d\n", n, illegal);
        
        int i;
        for (i = 0; i < n; ++i)
        {
            printf("%#010x", us[i]);
            if (isuchiness(us[i]))
                printf("    chiness");
            printf("\n");
        }

        max = n * 6 + 1;
        char *cs = calloc(max, 1);

        n = u8encode(us, cs, max, &illegal);
        printf("UTF-8 len: %zu, illegal: %d\n", n, illegal);
        puts(cs);

        free(us);
        free(cs);
    }

    return 0;
}

