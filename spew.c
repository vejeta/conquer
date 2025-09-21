// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * spew.c - NPC message generation system
 * 
 * This file is part of Conquer.
 * Originally Copyright (C) 1988-1989 by Edward M. Barlow and Adam Bryant
 * Copyright (C) 2025 Juan Manuel Méndez Rey (Vejeta) - Licensed under GPL v3 with permission from original authors
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "header.h"
#include "data.h"

#ifdef SPEW

/* Configuration parameters */
#define MAX_CLASSES 300
#define MAX_LINE_LEN 256
#define MAX_DEF_LEN 1000
#define ESCAPE_CHAR '\\'
#define DELIMITER_CHAR '/'
#define VARIANT_CHAR '|'

#ifndef DEFFILE
#define DEFFILE "rules"
#endif

/* Random number generator macro */
#define RANDOM(n) (rand() % (n))

/* Structure to hold a single definition within a class */
struct definition {
    int weight;                    /* cumulative weight for selection */
    char *text;                   /* the actual text definition */
    struct definition *next;      /* linked list pointer */
};

/* Structure to hold a class of definitions */
struct text_class {
    char *name;                   /* name of this class */
    char *variants;               /* string of variant tags */
    int total_weight;             /* total weight of all definitions */
    struct definition *defs;      /* linked list of definitions */
};

/* Global variables */
static FILE *rules_file = NULL;
static struct text_class *classes = NULL;
static int num_classes = 0;
static char input_line[MAX_LINE_LEN];
static const char *default_variants = " ";

/* Function prototypes */
static int load_rules_file(const char *filename);
static int parse_class_header(const char *line, struct text_class *cls);
static struct definition *parse_definition(const char *line);
static struct text_class *find_class(const char *name, int name_len);
static void generate_text(const char *class_spec, char variant_tag, FILE *output);
static void cleanup_memory(void);
static char *duplicate_string(const char *str);
static int read_line(void);
static int compare_classes(const void *a, const void *b);

/**
 * Main entry point for message generation
 * @param count Number of messages to generate
 * @param output File to write messages to
 */
void makemess(int count, FILE *output)
{
    char main_class[32];
    int i;

    if (!output) {
        fprintf(stderr, "Error: No output file provided\n");
        return;
    }

    /* Try to load the rules file */
    char filename[256];
    snprintf(filename, sizeof(filename), "%s/%s", DEFAULTDIR, DEFFILE);

    if (load_rules_file(filename) != 0) {
        fprintf(stderr, "Error: Cannot load rules file: %s\n", filename);
        return;
    }

    /* Generate the requested number of messages */
    strcpy(main_class, "MAIN/ ");
    for (i = 0; i < count; i++) {
        generate_text(main_class, ' ', output);
        if (i < count - 1) {
            fprintf(output, "\n");
        }
    }

    cleanup_memory();
}

/**
 * Load and parse the rules file
 */
static int load_rules_file(const char *filename)
{
    rules_file = fopen(filename, "r");
    if (!rules_file) {
        return -1;
    }

    /* Allocate memory for classes */
    classes = calloc(MAX_CLASSES, sizeof(struct text_class));
    if (!classes) {
        fclose(rules_file);
        return -1;
    }

    /* Read the first line - should start with '%' */
    if (!read_line() || input_line[0] != '%') {
        fprintf(stderr, "Error: Rules file must start with class definition\n");
        fclose(rules_file);
        return -1;
    }

    /* Parse all classes */
    while (input_line[1] != '%') {
        if (num_classes >= MAX_CLASSES) {
            fprintf(stderr, "Error: Too many classes (max %d)\n", MAX_CLASSES);
            fclose(rules_file);
            return -1;
        }

        /* Parse class header */
        if (parse_class_header(input_line, &classes[num_classes]) != 0) {
            fclose(rules_file);
            return -1;
        }

        /* Parse definitions for this class */
        struct definition **def_ptr = &classes[num_classes].defs;
        while (read_line() && input_line[0] != '%') {
            struct definition *def = parse_definition(input_line);
            if (!def) {
                continue;
            }

            *def_ptr = def;
            classes[num_classes].total_weight += def->weight;
            def->weight = classes[num_classes].total_weight; /* Make cumulative */
            def_ptr = &def->next;
        }

        num_classes++;
    }

    fclose(rules_file);

    /* Sort classes by name for binary search */
    qsort(classes, num_classes, sizeof(struct text_class), compare_classes);

    return 0;
}

/**
 * Parse a class header line
 */
static int parse_class_header(const char *line, struct text_class *cls)
{
    static char temp_name[100];
    static char temp_variants[100];
    const char *p = line + 1; /* Skip the '%' */
    char *name_ptr = temp_name;

    /* Initialize the class */
    cls->name = NULL;
    cls->variants = (char *)default_variants;
    cls->total_weight = 0;
    cls->defs = NULL;

    /* Skip whitespace */
    while (*p == ' ') p++;

    /* Extract class name */
    if (!isalnum(*p)) {
        fprintf(stderr, "Error: Invalid class name in: %s\n", line);
        return -1;
    }

    while (isalnum(*p)) {
        *name_ptr++ = *p++;
    }
    *name_ptr = '\0';
    cls->name = duplicate_string(temp_name);

    /* Look for variant tags */
    while (*p) {
        if (*p == ' ') {
            p++;
            continue;
        } else if (*p == '{') {
            p++;
            char *var_ptr = temp_variants;
            *var_ptr++ = ' '; /* Default variant */

            while (*p && *p != '}') {
                if (isalnum(*p)) {
                    *var_ptr++ = *p;
                }
                p++;
            }

            if (*p == '}') p++;
            *var_ptr = '\0';
            cls->variants = duplicate_string(temp_variants);
            break;
        } else {
            fprintf(stderr, "Error: Invalid class header: %s\n", line);
            return -1;
        }
    }

    return 0;
}

/**
 * Parse a definition line
 */
static struct definition *parse_definition(const char *line)
{
    struct definition *def;
    const char *p = line;
    int weight = 1; /* default weight */
    static char processed_text[MAX_DEF_LEN];
    char *out = processed_text;

    def = malloc(sizeof(struct definition));
    if (!def) {
        return NULL;
    }

    /* Check for weight specification */
    if (*p == '(') {
        p++;
        while (*p == ' ') p++;

        if (isdigit(*p)) {
            weight = 0;
            while (isdigit(*p)) {
                weight = weight * 10 + (*p - '0');
                p++;
            }
        }

        while (*p == ' ') p++;
        if (*p == ')') p++;
    }

    /* Process the text, handling escape sequences */
    while (*p && (out - processed_text) < MAX_DEF_LEN - 10) {
        if (*p == ESCAPE_CHAR) {
            *out++ = ESCAPE_CHAR;
            p++;
            if (isalnum(*p)) {
                /* Copy class reference */
                while (isalnum(*p)) {
                    *out++ = *p++;
                }
                *out++ = DELIMITER_CHAR;

                /* Handle variant tag */
                if (*p == DELIMITER_CHAR) {
                    p++;
                    if (isalnum(*p) || *p == ' ' || *p == '&') {
                        *out++ = *p++;
                    } else {
                        *out++ = ' ';
                    }
                } else {
                    *out++ = ' ';
                }
            } else if (*p == '!') {
                /* Newline escape */
                *out++ = '!';
                p++;
            } else if (*p) {
                /* Other escapes */
                *out++ = *p++;
            }
        } else {
            *out++ = *p++;
        }
    }

    *out = '\0';

    def->weight = weight;
    def->text = duplicate_string(processed_text);
    def->next = NULL;

    return def;
}

/**
 * Find a class by name using binary search
 */
static struct text_class *find_class(const char *name, int name_len)
{
    int low = 0, high = num_classes - 1;

    while (low <= high) {
        int mid = (low + high) / 2;
        int cmp = strncmp(name, classes[mid].name, name_len);

        if (cmp == 0 && classes[mid].name[name_len] == '\0') {
            return &classes[mid];
        } else if (cmp < 0) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    return NULL;
}

/**
 * Generate text from a class specification
 */
static void generate_text(const char *class_spec, char default_variant, FILE *output)
{
    const char *slash_pos = strchr(class_spec, DELIMITER_CHAR);
    if (!slash_pos) {
        fprintf(output, "???%s???", class_spec);
        return;
    }

    int name_len = slash_pos - class_spec;
    char variant_tag = slash_pos[1];
    if (variant_tag == '&') {
        variant_tag = default_variant;
    }

    /* Find the class */
    struct text_class *cls = find_class(class_spec, name_len);
    if (!cls) {
        fprintf(output, "???");
        fwrite(class_spec, 1, name_len, output);
        fprintf(output, "???");
        return;
    }

    /* Find variant index */
    int variant_idx = 0;
    if (cls->variants) {
        const char *var_pos = strchr(cls->variants, variant_tag);
        if (var_pos) {
            variant_idx = var_pos - cls->variants;
        }
    }

    /* Select a random definition */
    if (cls->total_weight == 0) {
        return;
    }

    int rand_val = RANDOM(cls->total_weight);
    struct definition *def = cls->defs;
    while (def && def->weight <= rand_val) {
        def = def->next;
    }

    if (!def) {
        return;
    }

    /* Process the definition text */
    const char *p = def->text;
    int in_variants = 0;
    int writing = 1;
    int current_variant = 0;

    while (*p) {
        if (*p == ESCAPE_CHAR) {
            p++;
            if (*p == '!') {
                if (writing) {
                    fprintf(output, "\n");
                }
                p++;
            } else if (isalnum(*p)) {
                /* Recursive class reference */
                if (writing) {
                    const char *start = p - 1;
                    while (*p != DELIMITER_CHAR && *p) p++;
                    if (*p == DELIMITER_CHAR) {
                        p += 2; /* Skip delimiter and variant tag */
                        char temp_spec[64];
                        int spec_len = (p - start);
                        if (spec_len < sizeof(temp_spec)) {
                            memcpy(temp_spec, start, spec_len);
                            temp_spec[spec_len] = '\0';
                            generate_text(temp_spec, default_variant, output);
                        }
                    }
                } else {
                    /* Skip over the reference */
                    while (*p != DELIMITER_CHAR && *p) p++;
                    if (*p == DELIMITER_CHAR) p += 2;
                }
            } else if (*p) {
                if (writing) {
                    fputc(*p, output);
                }
                p++;
            }
        } else if (*p == '{') {
            if (!in_variants) {
                in_variants = 1;
                writing = (variant_idx == 0);
                current_variant = 0;
            } else if (writing) {
                fputc('{', output);
            }
            p++;
        } else if (*p == VARIANT_CHAR) {
            if (in_variants) {
                current_variant++;
                writing = (variant_idx == current_variant);
            } else if (writing) {
                fputc(VARIANT_CHAR, output);
            }
            p++;
        } else if (*p == '}') {
            if (in_variants) {
                writing = 1;
                in_variants = 0;
            } else if (writing) {
                fputc('}', output);
            }
            p++;
        } else {
            if (writing) {
                fputc(*p, output);
            }
            p++;
        }
    }
}

/**
 * Read a line from the rules file, skipping comments and empty lines
 */
static int read_line(void)
{
    char *comment_pos;

    do {
        if (!fgets(input_line, MAX_LINE_LEN, rules_file)) {
            strcpy(input_line, "%%"); /* EOF marker */
            return 0;
        }

        /* Remove newline */
        char *newline = strrchr(input_line, '\n');
        if (newline) *newline = '\0';

        /* Remove comments (marked by \*) */
        comment_pos = strstr(input_line, "\\*");
        if (comment_pos) {
            *comment_pos = '\0';
        }

        /* Trim trailing whitespace */
        int len = strlen(input_line);
        while (len > 0 && isspace(input_line[len-1])) {
            input_line[--len] = '\0';
        }

    } while (input_line[0] == '\0');

    return 1;
}

/**
 * Compare function for sorting classes by name
 */
static int compare_classes(const void *a, const void *b)
{
    const struct text_class *cls_a = (const struct text_class *)a;
    const struct text_class *cls_b = (const struct text_class *)b;
    return strcmp(cls_a->name, cls_b->name);
}

/**
 * Duplicate a string
 */
static char *duplicate_string(const char *str)
{
    if (!str) return NULL;

    int len = strlen(str);
    char *copy = malloc(len + 1);
    if (copy) {
        strcpy(copy, str);
    }
    return copy;
}

/**
 * Clean up allocated memory
 */
static void cleanup_memory(void)
{
    if (!classes) return;

    for (int i = 0; i < num_classes; i++) {
        free(classes[i].name);
        if (classes[i].variants != default_variants) {
            free(classes[i].variants);
        }

        struct definition *def = classes[i].defs;
        while (def) {
            struct definition *next = def->next;
            free(def->text);
            free(def);
            def = next;
        }
    }

    free(classes);
    classes = NULL;
    num_classes = 0;
}

#else
/* If SPEW is not defined, provide stub implementation */
void makemess(int count, FILE *output)
{
    /* Do nothing if SPEW is disabled */
}
#endif /* SPEW */