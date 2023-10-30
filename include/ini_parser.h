#ifndef INI_PARSER_H
#define INI_PARSER_H

#ifndef INI_PARSER_ISIZE_TYPE
#include <stdint.h>
#define INI_PARSER_ISIZE_TYPE int32_t
#endif
#ifndef INI_PARSER_USIZE_TYPE
#include <stdint.h>
#define INI_PARSER_USIZE_TYPE uint32_t
#endif

struct StringSlice
{
    const char *begin;
    INI_PARSER_USIZE_TYPE len;
};

enum Result
{
    Ok,
    NotFound,
    ErrorEqualSign,
    WrongKey,
};

enum Result read_ini_value(const char *ini_file, const char *section, const char *key, struct StringSlice *output);

#endif