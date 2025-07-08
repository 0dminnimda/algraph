#ifndef STR_H
#define STR_H

#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "types.h"

struct str {
    char *data = NULL;
    u64 length = 0;

    char &operator[](u64 index);
    bool operator==(const str& other) const;
};

/* Usually have null terminators, except for slices
 * or strings constructed from non-null-terminated strings.
 * On copy the null terminator is always added.
 * But it's never counted towards the length.
 */


#define PRI_str "%.*s"
#define FMT_str(self) (int)(self).length, (self).data
#define FMT_str_max(self, max) ((self).length > (max)? (max) : (int)(self).length), (self).data
#define str_NULL (str){NULL, 0}
#define PRI_var_char "%*.c"
#define FMT_var_char(char, count) (int)(count), (char)

char *begin(str&);
char *end(str&);

str str_slice(str self, u64 start, u64 end);

str str_cstr_view(char *data);
#define str(lit) (str){ (lit), sizeof((lit)) - 1 /* Don't count the \0 */ }
void str_free(str self);

str str_copy_known_length(const char *data, u64 length);
str str_copy_cstr(const char *cstr);
str str_copy(str self);

#define str_add(...) str_add_array((str[]){__VA_ARGS__}, sizeof((str[]){__VA_ARGS__})/sizeof(str))
str str_add_array(const str *items, u64 count);

int str_compare(str self, str other);

bool str_startswith(str self, str other);

/* Returns a slice */
char *str_after_strip(char *pos, char *end, char to_strip);
char *str_after_whitespace_strip(char *pos, char *end);
char *str_find_next_line(char *pos, char *end);

/* Returns a "word". Several spaces are treated as one separator. */
str str_tokenize_whitespace(str *self);

str read_entire_file(str file_path);

str str_format_raw(u64 reserve_for_data, str format, ...);
#define str_format(reserve_for_data, s, ...) \
    str_format_raw(reserve_for_data, str(s) __VA_OPT__(,) __VA_ARGS__)

void str_debug_fprint(str self, FILE *stream);

#endif  // STR_H


#ifdef STR_IMPLEMENT

#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>

char *begin(str &self) { return self.data; }
char *end(str &self) { return self.data + self.length; }

char *string_duplicate(const char *source) {
    u64 len = strlen(source) + 1;
    char *copy = (char *)malloc(len * sizeof(char));
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, source, len);
    return copy;
}

char *string_duplicate_length(const char *source, u64 length) {
    char *copy = (char *)malloc((length + 1) * sizeof(char));
    if (copy == NULL) {
        return NULL;
    }
    strncpy(copy, source, length);
    copy[length] = '\0';
    return copy;
}

char *string_duplicate_known_length(const char *source, u64 length) {
    char *copy = (char *)malloc((length + 1) * sizeof(char));
    if (copy == NULL) {
        return NULL;
    }
    memcpy(copy, source, length);
    copy[length] = '\0';
    return copy;
}

str str_slice(str self, u64 start, u64 end) {
    assert(start <= self.length && "'start' is out of bounds");
    assert(end <= self.length && "'end' is out of bounds");
    assert(start <= end && "'start' is after the 'end'");
    return (str){self.data + start, end - start};
}

char &str::operator[](u64 index) {
    assert((index < length) && "Index out of bounds");
    return data[index];
}

str str_cstr_view(char *data) {
    return (str){data, strlen(data)};
}

void str_free(str self) { free(self.data); }

str str_copy_known_length(const char *data, u64 length) {
    return (str){string_duplicate_known_length(data, length), length};
}

str str_copy_cstr(const char *cstr) {
    return str_copy_known_length(cstr, strlen(cstr));
}

str str_copy(str self) {
    return str_copy_known_length(self.data, self.length);
}

str str_add_array(const str *items, u64 count) {
    u64 len = 0;
    for (u64 i = 0; i < count; i++) {
        len += items[i].length;
    }
    str result = {(char*)malloc((len + 1) * sizeof(char)), len};
    assert((result.data != NULL) && "Buy more RAM lol!");
    len = 0;
    for (u64 i = 0; i < count; i++) {
        memcpy(result.data + len, items[i].data, items[i].length);
        len += items[i].length;
    }
    result.data[len] = 0;
    return result;
}

int str_compare(str self, str other) {
    if (self.length < other.length) {
        return -1;
    } else if (self.length > other.length) {
        return 1;
    } else if (self.length == 0) {
        return 0;
    } else {
        return memcmp(self.data, other.data, self.length);
    }
}

bool str::operator==(const str& other) const {
    return str_compare(*this, other) == 0;
}

bool str_startswith(str self, str other) {
    if (self.length < other.length) {
        return false;
    }
    return memcmp(self.data, other.data, other.length) == 0;
}

char *str_after_strip(char *pos, char *end, char to_strip) {
    for (; pos < end; pos++) {
        if (*pos != to_strip) break;
    }
    return pos;  // pos == end
}

char *str_after_whitespace_strip(char *pos, char *end) {
    for (; pos < end; pos++) {
        if (!isspace(*pos)) break;
    }
    return pos;  // pos == end
}

char *str_find_next_line(char *pos, char *end) {
    while (pos < end) {
        if (*pos == '\n') {
            pos++;
            return pos;
        } else if (*pos == '\r') {
            pos++;
            if ((pos < end) && (*pos == '\n')) {
                pos++;
            }
            return pos;
        } else {
            pos++;
        }
    }
    return end;
}

str str_tokenize_whitespace(str *self) {
    u64 word_length = self->length;
    for (char &c : *self) {
        if (isspace(c)) {
            word_length = &c - self->data;
            break;
        }
    }
    str word = {self->data, word_length};
    self->data += word_length;
    self->length -= word_length;

    u64 space_length = self->length;
    for (char &c : *self) {
        if (!isspace(c)) {
            space_length = &c - self->data;
            break;
        }
    }
    self->data += space_length;
    self->length -= space_length;
    return word;
}

/* TODO: find, replace, count, etc.
 * SEE:
 *   https://github.com/lattera/glibc/blob/master/string/strstr.c
 *   https://github.com/python/cpython/tree/main/Objects/stringlib
 *   https://stackoverflow.com/questions/3183582/what-is-the-fastest-substring-search-algorithm
 *   https://smart-tool.github.io/smart/
 *   https://web.archive.org/web/20200420024210/http://www.dmi.unict.it/~faro/smart/algorithms.php
 *   https://www-igm.univ-mlv.fr/~lecroq/string/index.html
 *   https://web.archive.org/web/20170829195535/http://www.stupros.com/site/postconcept/Breslauer-Grossi-Mignosi.pdf
 *   https://www.cs.haifa.ac.il/%7Eoren/Publications/bpsm.pdf
 *   https://www.cs.utexas.edu/~moore/publications/sustik-moore.pdf
 */


str read_entire_file(str file_path) {
    FILE *f = fopen(file_path.data, "rb");
    if (f == NULL) {
        fprintf(stderr, "Could not open file: " PRI_str "\n", FMT_str(file_path));
        return str_NULL;
    }
    fseek(f, 0, SEEK_END);
    u64 fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *cstr = (char *)malloc(fsize + 1);
    assert((cstr != NULL) && "Buy more RAM lol!");

    size_t read = fread(cstr, 1, fsize, f);
    fclose(f);

    if (read != fsize) {
        fprintf(stderr, "File (" PRI_str ") changed size wihle reading\n", FMT_str(file_path));
        free(cstr);
        return str_NULL;
    }

    cstr[fsize] = 0;
    return (str){cstr, fsize};
}

str str_format_raw(u64 to_add, str format, ...) {
    u64 buffa_siz = format.length + to_add;
    char buffa[buffa_siz];

    va_list argp;
    va_start(argp, format);
    u64 len = vsnprintf(buffa, buffa_siz, format.data, argp);
    va_end(argp);

    return str_copy_known_length(buffa, len);
}

void str_debug_fprint(str self, FILE *stream) {
    for (char &c : self) {
        fprintf(stream, "[%zu] = '%c' (%d)\n", (&c - self.data), c, c);
    }
}

#endif  // STR_IMPLEMENT
