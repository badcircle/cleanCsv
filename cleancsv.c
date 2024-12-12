#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <windows.h>

#define MAX_LINE_LENGTH 65535

// Function to get base filename without extension
void get_base_filename(char *dest, const char *src) {
    char *last_slash = strrchr(src, '\\');
    if (!last_slash) last_slash = strrchr(src, '/');
    char *start = last_slash ? last_slash + 1 : (char *)src;
    char *dot = strrchr(start, '.');
    
    if (dot) {
        strncpy(dest, start, dot - start);
        dest[dot - start] = '\0';
    } else {
        strcpy(dest, start);
    }
}

// Function to trim whitespace from both ends of a string
void trim(char *str) {
    char *start = str;
    char *end = str + strlen(str) - 1;

    // Trim leading whitespace
    while (isspace(*start)) start++;

    // Trim trailing whitespace
    while (end > start && isspace(*end)) end--;

    // Null terminate the trimmed string
    *(end + 1) = '\0';

    // Move trimmed string to the beginning if needed
    if (start != str) {
        memmove(str, start, end - start + 2);
    }
}

// Function to escape special characters for MySQL
void escape_for_mysql(char *dest, const char *src) {
    char *d = dest;
    const char *s = src;
    *d++ = '"';  // Start with quote

    while (*s) {
        switch (*s) {
            case '"':  // Escape double quotes
            case '\\': // Escape backslashes
                *d++ = '\\';
                *d++ = *s;
                break;
            default:
                *d++ = *s;
        }
        s++;
    }

    *d++ = '"';  // End with quote
    *d = '\0';   // Null terminate
}

int main(int argc, char *argv[]) {
    // Set console to UTF-8 on Windows
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Check if filename is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_csv_file>\n", argv[0]);
        return 1;
    }

    // Open input file in binary mode to preserve encoding
    FILE *input_file = fopen(argv[1], "rb");
    if (!input_file) {
        perror("Error opening input file");
        return 1;
    }

    // Create output filename with base name + suffix
    char base_filename[MAX_LINE_LENGTH];
    get_base_filename(base_filename, argv[1]);
    char output_filename[MAX_LINE_LENGTH];
    snprintf(output_filename, sizeof(output_filename), "%s_mysql.csv", base_filename);

    // Open output file in binary mode for UTF-8
    FILE *output_file = fopen(output_filename, "wb");
    if (!output_file) {
        perror("Error creating output file");
        fclose(input_file);
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    char escaped_field[MAX_LINE_LENGTH * 2];
    int is_first_row = 1;

    // Process each line
    while (fgets(line, sizeof(line), input_file)) {
        char processed_line[MAX_LINE_LENGTH] = {0};
        char *line_ptr = processed_line;
        char *cursor = line;
        int first_field = 1;

        while (*cursor && *cursor != '\n' && *cursor != '\r') {
            // Skip leading whitespace
            while (isspace(*cursor)) cursor++;

            // Start of a quoted field
            if (*cursor == '"') {
                char *field_start = cursor + 1;
                char *field_end = field_start;
                bool in_quotes = true;

                // Find end of quoted field, handling escaped quotes
                while (*field_end) {
                    if (*field_end == '"' && *(field_end + 1) != '"') {
                        in_quotes = false;
                        break;
                    }
                    // Handle double quotes within quoted field
                    if (*field_end == '"' && *(field_end + 1) == '"') {
                        field_end++;
                    }
                    field_end++;
                }

                // Extract field content
                if (in_quotes) {
                    fprintf(stderr, "Unclosed quote in CSV\n");
                    break;
                }

                // Create null-terminated field
                size_t field_len = field_end - field_start;
                char field[MAX_LINE_LENGTH] = {0};
                strncpy(field, field_start, field_len);

                // Replace double quotes with single quotes
                char *p = field;
                while ((p = strstr(p, "\"\"")) != NULL) {
                    memmove(p, p + 1, strlen(p + 1) + 1);
                }

                // Prepare escaped field
                if (!first_field) {
                    line_ptr += sprintf(line_ptr, ",");
                }
                escape_for_mysql(escaped_field, field);
                line_ptr += sprintf(line_ptr, "%s", escaped_field);

                // Move cursor past closing quote and comma
                cursor = field_end + 1;
                if (*cursor == ',') cursor++;
                first_field = 0;
            } 
            // Non-quoted field
            else {
                char *field_start = cursor;
                char *field_end = cursor;

                // Find end of field
                while (*field_end && *field_end != ',' && *field_end != '\n' && *field_end != '\r') {
                    field_end++;
                }

                // Create null-terminated field
                char field[MAX_LINE_LENGTH];
                strncpy(field, field_start, field_end - field_start);
                field[field_end - field_start] = '\0';

                // Trim the field
                trim(field);

                // Prepare escaped field
                if (!first_field) {
                    line_ptr += sprintf(line_ptr, ",");
                }

                // Escape and add field
                escape_for_mysql(escaped_field, field);
                line_ptr += sprintf(line_ptr, "%s", escaped_field);

                // Move cursor
                cursor = field_end;
                if (*cursor == ',') cursor++;
                first_field = 0;
            }

            // Break if end of line
            if (*cursor == '\n' || *cursor == '\r' || *cursor == '\0') break;
        }

        // Ensure line ends with newline
        strcat(processed_line, "\n");

        // Print first row (for reference)
        if (is_first_row) {
            printf("First row: %s", processed_line);
            is_first_row = 0;
        }

        // Write processed line to output file
        fprintf(output_file, "%s", processed_line);
    }

    fclose(input_file);
    fclose(output_file);

    printf("MySQL-optimized CSV created: %s\n", output_filename);

    return 0;
}