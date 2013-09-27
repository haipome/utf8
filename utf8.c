/*
 * Description: 
 *     History: damonyang@tencent.com, 2013/05/29, create
 */


# include <stdint.h>
# include <stddef.h>

# include "utf8.h"

int getu8c(char **src, ucs4_t *uc)
{
    static char umap[256] = { 0 };
    static int  umap_init_flag = 0;

    if (umap_init_flag == 0)
    {
        int i;

        for (i = 0; i < 0x100; ++i)
        {
            if (i < 0x80)
            {
                umap[i] = 1;
            }
            else if (i >= 0xc0 && i < 0xe0)
            {
                umap[i] = 2;
            }
            else if (i >= 0xe0 && i < 0xf0)
            {
                umap[i] = 3;
            }
            else if (i >= 0xf0 && i < 0xf8)
            {
                umap[i] = 4;
            }
            else if (i >= 0xf8 && i < 0xfc)
            {
                umap[i] = 5;
            }
            else if (i >= 0xfc && i < 0xfe)
            {
                umap[i] = 6;
            }
            else
            {
                umap[i] = 0;
            }
        }

        umap_init_flag = 1;
    }

    uint8_t *s = (uint8_t *)(*src);
    int illegal = 0;

    while (umap[*s] == 0)
    {
        ++s;
        ++illegal;
    }

    int byte_num = umap[*s];
    uint32_t _utf8_char = *s++ & (0xff >> byte_num);

    int i;
    for (i = 1; i < byte_num; ++i)
    {
        if (*s == 0 || umap[*s])
        {
            *src = (char *)s;
            *uc = 0;

            return -1;
        }
        else
        {
            _utf8_char = (_utf8_char << 6) + (*s & 0x3f);
            s += 1;
        }
    }

    *src = (char *)s;
    *uc = _utf8_char;

    return illegal;
}

int u8decode(char *str, ucs4_t *des, size_t *n)
{
    if (*n < 1)
        return 0;

    char *s = str;
    size_t i = 0;
    int illegal = 0;

    while (*s && (i < (*n - 1)))
    {
        char *_s = s;
        int ret = getu8c(&s, des + i);
        
        if (ret < 0)
        {
            illegal += s - _s;
        }
        else
        {
            illegal += ret;

            ++i;
        }
    }

    des[i] = 0;
    *n = i;

    return illegal;
}

# define IF_LESS(n) do { \
    size_t _n = (n); \
    if (*left < (_n + 1)) return -2; \
    *left -= _n; \
} while (0)

int putu8c(ucs4_t uc, char **des, size_t *left)
{
    if (uc < 0)
        return -1;

    if (uc < (0x1 << 7))
    {
        IF_LESS(1);

        **des = (char)uc;
        *des += 1;
        **des = 0;

        return 0;
    }

    int byte_num = 0;

    if (uc < (0x1 << 11))
    {
        byte_num = 2;
    }
    else if (uc < (0x1 << 16))
    {
        byte_num = 3;
    }
    else if (uc < (0x1 << 21))
    {
        byte_num = 4;
    }
    else if (uc < (0x1 << 26))
    {
        byte_num = 5;
    }
    else if (uc < (0x1 << 31))
    {
        byte_num = 6;
    }
    else
    {
        return -3;
    }

    IF_LESS(byte_num);

    int i;
    for (i = byte_num - 1; i > 0; --i)
    {
        *(uint8_t *)(*des + i) = (uc & 0x3f) | 0x80;
        uc >>= 6;
    }

    *(uint8_t *)(*des) = uc | (0xff << (8 - byte_num));

    *des += byte_num;
    *(*des + byte_num) = 0;

    return 0;
}

int u8encode(ucs4_t *us, char *des, size_t *n)
{
    char *s = des;
    size_t left = *n;
    int illegal = 0;

    while (*us)
    {
        int ret = putu8c(*us, &s, &left);

        if (ret == -2)
        {
            *s = 0;

            return -1;
        }
        else if (ret < 0)
        {
            ++illegal;
        }
        
        ++us;
    }

    *s = 0;
    *n = *n - left;

    return illegal;
}

/* 中日韩越统一表意文字 */
int isuchiness(ucs4_t uc)
{
    /* 最初期统一汉字 */
    if (uc >= 0x4e00 && uc <= 0x9fcc)
        return 1;

    /* 扩展 A 区 */
    if (uc >= 0x3400 && uc <= 0x4db5)
        return 2;

    /* 扩展 B 区 */
    if (uc >= 0x20000 && uc <= 0x2a6d6)
        return 3;

    /* 扩展 C 区 */
    if (uc >= 0x2a700 && uc <= 0x2b734)
        return 4;

    /* 扩展 D 区 */
    if (uc >= 0x2b740 && uc <= 0x2b81f)
        return 5;

    /* 扩展 E 区 */
    if (uc >= 0x2b820 && uc <= 0x2f7ff)
        return 6;

    /* 台湾兼容汉字 */
    if (uc >= 0x2f800 && uc <= 0x2fa1d)
        return 7;

    /* 北朝鲜兼容汉字 */
    if (uc >= 0xfa70 && uc <= 0xfad9)
        return 8;

    /* 兼容汉字 */
    if (uc >= 0xf900 && uc <= 0xfa2d)
        return 9;

    /* 兼容汉字 */
    if (uc >= 0xfa30 && uc <= 0xfa6d)
        return 10;

    return 0;
}

# ifdef TEST

# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <error.h>

int main()
{
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1)
    {
        line[strlen(line) - 1] = 0;

        size_t len = strlen(line) + 1;
        ucs4_t *us = malloc(len * sizeof(ucs4_t));

        u8decode(line, us, &len);

        printf("len: %zu\n", len);
        
        int i;
        for (i = 0; i < len; ++i)
            printf("%#010x\n", us[i]);

        len = len * 6 + 1;
        char *cs = malloc(len);

        u8encode(us, cs, &len);

        printf("len: %zu\n", len);
        puts(cs);

        free(us);
        free(cs);
    }

    return 0;
}

# endif

