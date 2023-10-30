#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "ini_parser.h"

typedef INI_PARSER_ISIZE_TYPE isize;
typedef INI_PARSER_USIZE_TYPE usize;

bool stringslice_eqa(const struct StringSlice *a, const struct StringSlice *b)
{
    return a->len == b->len && strncmp(a->begin, b->begin, a->len) == 0;
}

bool stringslice_eq(const struct StringSlice *a, const char *b)
{
    return a->len == strlen(b) && strncmp(a->begin, b, a->len) == 0;
}

struct StringSlice trim(struct StringSlice s)
{

    if (s.len == 0)
    {
        return s;
    }
    usize start_index = 0;

    while (isspace(*(s.begin + start_index)))
    {
        start_index += 1;
        if (start_index >= s.len)
        {
            return (struct StringSlice){
                s.begin, 0};
        }
    }

    usize end_index = s.len - 1;

    while (isspace(*(s.begin + end_index)))
    {
        end_index -= 1;
    }

    usize new_len = end_index - start_index + 1;

    return (struct StringSlice){
        s.begin + start_index, new_len};
}

enum Result try_match_on_line(struct StringSlice s, const char *key, struct StringSlice *output)
{
    usize equalsign_index = strcspn(s.begin, "=");

    if (equalsign_index == 0 || equalsign_index >= s.len - 1)
    {
        // = is at beginning of line or like "aoesuth="
        return ErrorEqualSign;
    }

    // "aoeu = uea"
    // "0123456789"

    struct StringSlice key_part = trim((struct StringSlice){s.begin, equalsign_index});

    if (stringslice_eq(&key_part, key))
    {
        *output = trim((struct StringSlice){s.begin + equalsign_index + 1, s.len - equalsign_index - 1});
        return Ok;
    }
    else
    {
        return WrongKey;
    }
}

enum Result read_ini_value(const char *ini_file, const char *section, const char *key, struct StringSlice *output)
{

    const char *current_line_begin = ini_file;
    usize current_line_len = strcspn(current_line_begin, "\n");

    bool correct_section = false;

    while (true)
    {
        struct StringSlice current_line = {current_line_begin, current_line_len};

        struct StringSlice current_line_trimmed = trim(current_line);

        if (current_line_trimmed.len <= 2)
        {
            goto next_line;
        }

        switch (current_line_trimmed.begin[0])
        {
        case '[':
        {
            struct StringSlice section_name_part = {current_line_trimmed.begin + 1, current_line_trimmed.len - 2};
            if (stringslice_eq(&section_name_part, section))
            {
                correct_section = true;
            }
            else
            {
                correct_section = false;
            }
            break;
        }
        case ';':
        case '#':
            break;
        default:
        {
            if (correct_section)
            {
                struct StringSlice tmp_output = {NULL, 0};
                enum Result res = try_match_on_line(current_line, key, &tmp_output);
                switch (res)
                {
                case WrongKey:
                    break;
                case Ok:
                    *output = tmp_output;
                    return Ok;
                default:
                    return res;
                }
            }
            break;
        }
        }

    next_line:

        if (strlen(current_line.begin) == current_line.len)
        {
            // this is the last line
            break;
        }
        current_line_begin += current_line_len + 1;

        current_line_len = strcspn(current_line_begin, "\n");
    }

    return NotFound;
}