/*
 * Description: 
 *     History: damonyang@tencent.com, 2013/05/29, create
 */

# pragma once

# include <stdint.h>
# include <stddef.h>

/*
 * 标准 C 并没有规定 wchar_t 的位数。
 * 但 GNU C Lib 保证 wchar_t 是 32 位，所以
 * 你可以用 wchar.h 中定义的函数来像 wchar_t
 * 一样操纵 ucs4_t.
 * 
 * http://www.gnu.org/software/libc/manual/html_node/Extended-Char-Intro.html
 */
typedef int32_t ucs4_t;

ucs4_t getu8c(char **src, int *illegal);
size_t u8decode(char const *str, ucs4_t *des, size_t n, int *illegal);

int putu8c(ucs4_t uc, char **des, size_t *left);
size_t u8encode(ucs4_t *us, char *des, size_t n, int *illegal);

int isuchiness(ucs4_t uc);

ucs4_t ufull2half(ucs4_t uc);

int ishiragana(ucs4_t uc);
int iskatakana(ucs4_t uc);

