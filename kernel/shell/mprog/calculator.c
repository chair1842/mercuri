#include <stdbool.h>
#include <string.h>
#include <vgatxm.h>
#include <driver/keyboard.h>

static const char* input;

// Forward declarations for the parser:
static int  parse_expression(void);
static int  parse_term(void);
static int  parse_factor(void);
static void skip_whitespace(void);
static int  parse_number(void);

// Read a full line from the keyboard (blocking)
void read_line(char* buf, int max_len) {
    int i = 0;
    while (i < max_len - 1) {
        char c = keyboard_get_char();

        if (c == '\n') break;
        if (c == '\b') {
            if (i > 0) {
                i--;
                print_char('\b');
            }
            continue;
        }
        if (c >= ' ') {
            buf[i++] = c;
            print_char(c);
        }
    }
    buf[i] = '\0';
    print_char('\n');
}

void calc_command(const char* arg) {
    // Skip the “calc” keyword and any whitespace
    while (*arg == ' ' || *arg == '\t') arg++;

    // If user typed “calc” with no args, or “calc help”:
    if (*arg == '\0' || strcmp(arg, "help") == 0) {
        print_string("Usage: calc <expression>\n");
        print_string("Examples:\n  calc 1+2\n  calc (3+4)*5\n");
        if (*arg == '\0') {
            // Interactive prompt
            print_string("Enter expression: ");
            char expr[64];
            read_line(expr, sizeof(expr));
            input = expr;
        } else {
            return;
        }
    } else {
        // Inline expression mode:
        input = arg;
    }

    // Now evaluate whatever is in `input`
    skip_whitespace();
    if (*input == '\0') {
        print_string("Error: no expression provided\n");
        return;
    }

    // Evaluate with full precedence and parentheses
    int result = parse_expression();
    print_string("= ");
    print_int(result);
    print_char('\n');
}

// ——————————————————————————————————————————————————————
// Parser implementation (unchanged from before)
// ——————————————————————————————————————————————————————

static void skip_whitespace(void) {
    while (*input == ' ' || *input == '\t') input++;
}

static int parse_number(void) {
    skip_whitespace();
    bool neg = false;
    if (*input == '+' || *input == '-') {
        neg = (*input == '-');
        input++;
    }
    int value = 0;
    if (*input < '0' || *input > '9') return 0;
    while (*input >= '0' && *input <= '9') {
        value = value * 10 + (*input - '0');
        input++;
    }
    return neg ? -value : value;
}

static int parse_factor(void) {
    skip_whitespace();
    if (*input == '(') {
        input++;
        int v = parse_expression();
        if (*input == ')') input++;
        return v;
    }
    return parse_number();
}

static int parse_term(void) {
    int v = parse_factor();
    while (1) {
        skip_whitespace();
        char op = *input;
        if (op != '*' && op != '/') break;
        input++;
        int rhs = parse_factor();
        if (op == '*')     v *= rhs;
        else if (rhs != 0) v /= rhs;
        else {
            print_string("Error: divide by zero\n");
            return 0;
        }
    }
    return v;
}

static int parse_expression(void) {
    int v = parse_term();
    while (1) {
        skip_whitespace();
        char op = *input;
        if (op != '+' && op != '-') break;
        input++;
        int rhs = parse_term();
        if (op == '+') v += rhs;
        else           v -= rhs;
    }
    return v;
}