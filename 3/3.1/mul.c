#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct multiplier;

enum multiplier_state_enum {
    multiplier_mul,
    multiplier_parentheses_open,
    multiplier_parentheses_close,
    multiplier_number_one,
    multiplier_comma,
    multiplier_number_two,
    multiplier_complete,
    multiplier_end
};

typedef void (*next_state_function)(struct multiplier* scanner);

struct multiplier_state {
    enum multiplier_state_enum state;
    next_state_function next_state;
};

struct multiplier {
    char* buffer;
    struct multiplier_state state;
    int number_one;
    int number_two;
    int total_multiplications;
};

void next_state_mul(struct multiplier* scanner);
void next_state_parentheses_open(struct multiplier* scanner);
void next_state_parentheses_close(struct multiplier* scanner);
void next_state_number_one(struct multiplier* scanner);
void next_state_comma(struct multiplier* scanner);
void next_state_number_two(struct multiplier* scanner);
void next_state_complete(struct multiplier* scanner);

struct multiplier_state states[] = {
    { multiplier_mul, next_state_mul },
    { multiplier_parentheses_open, next_state_parentheses_open },
    { multiplier_parentheses_close, next_state_parentheses_close },
    { multiplier_number_one, next_state_number_one },
    { multiplier_comma, next_state_comma },
    { multiplier_number_two, next_state_number_two },
    { multiplier_complete, next_state_complete },
    { multiplier_end, NULL }
};

void next_state_mul(struct multiplier* scanner)
{
    char *mul = strstr(scanner->buffer, "mul");
    if (mul != NULL) {
        scanner->buffer = mul + 3;
        scanner->state = states[multiplier_parentheses_open];
    } else {
        scanner->state = states[multiplier_end];
    }
}

void next_state_parentheses_open(struct multiplier* scanner)
{
    if (*(scanner->buffer)) {
        scanner->buffer++;
        scanner->number_one = 0;
        scanner->number_two = 0;
        scanner->state = states[multiplier_number_one];
    } else {
        scanner->state = states[multiplier_mul];
    }
}

void next_state_number_one(struct multiplier* scanner)
{
    int digit_count = 0;

    while(isdigit(*(scanner->buffer))) {
        scanner->number_one = scanner->number_one * 10 + (*(scanner->buffer) - '0');
        scanner->buffer++;
        digit_count++;
    }

    if (digit_count > 0 && digit_count <= 3) {
        scanner->state = states[multiplier_comma];
    } else {
        scanner->state = states[multiplier_mul];
    }
}

void next_state_comma(struct multiplier* scanner)
{
    if (*(scanner->buffer) == ',') {
        scanner->buffer++;
        scanner->state = states[multiplier_number_two];
    } else {
        scanner->state = states[multiplier_mul];
    }
}

void next_state_number_two(struct multiplier* scanner)
{
    int digit_count = 0;

    while(isdigit(*(scanner->buffer))) {
        scanner->number_two = scanner->number_two * 10 + (*(scanner->buffer) - '0');
        scanner->buffer++;
        digit_count++;
    }

    if (digit_count > 0 && digit_count <= 3) {
        scanner->state = states[multiplier_parentheses_close];
    } else {
        scanner->state = states[multiplier_mul];
    }
}

void next_state_parentheses_close(struct multiplier* scanner)
{
    if (*(scanner->buffer) == ')') {
        scanner->buffer++;
        scanner->state = states[multiplier_complete];
    } else {
        scanner->state = states[multiplier_mul];
    }
}

void next_state_complete(struct multiplier* scanner)
{
    scanner->total_multiplications += scanner->number_one * scanner->number_two;
    scanner->state = states[multiplier_mul];
}

int main(void)
{
    char buffer[BUFSIZ];

    int total_multiplications = 0;

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        struct multiplier scanner;
        scanner.state = states[multiplier_mul];
        scanner.buffer = buffer;
        scanner.total_multiplications = 0;

        while (scanner.state.state != multiplier_end) {
            scanner.state.next_state(&scanner);
        }

        total_multiplications += scanner.total_multiplications;
    }
    
    printf("Total of multiplications = %d\n", total_multiplications);
}