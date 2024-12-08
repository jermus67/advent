#include <stdio.h>
#include <string.h>
#include <ctype.h>

struct multiplier;

enum multiplier_state_enum {
    multiplier_start,
    multiplier_mul,
    multiplier_parentheses_open,
    multiplier_parentheses_close,
    multiplier_number_one,
    multiplier_comma,
    multiplier_number_two,
    multiplier_complete,
    multiplier_end
};

typedef void (*next_state_function)(struct multiplier* m);

struct multiplier_state {
    enum multiplier_state_enum state;
    next_state_function next_state;
};

struct multiplier {
    char* buffer;
    struct multiplier_state state;
    int mul_enabled;
    int number_one;
    int number_two;
    int total_multiplications;
};

void next_state_start(struct multiplier* m);
void next_state_mul(struct multiplier* m);
void next_state_parentheses_open(struct multiplier* m);
void next_state_parentheses_close(struct multiplier* m);
void next_state_number_one(struct multiplier* m);
void next_state_comma(struct multiplier* m);
void next_state_number_two(struct multiplier* m);
void next_state_complete(struct multiplier* m);

struct multiplier_state states[] = {
    { multiplier_start, next_state_start },
    { multiplier_mul, next_state_mul },
    { multiplier_parentheses_open, next_state_parentheses_open },
    { multiplier_parentheses_close, next_state_parentheses_close },
    { multiplier_number_one, next_state_number_one },
    { multiplier_comma, next_state_comma },
    { multiplier_number_two, next_state_number_two },
    { multiplier_complete, next_state_complete },
    { multiplier_end, NULL }
};

void next_state_start(struct multiplier* m)
{
    char *start = strstr(m->buffer, "do()");
    if (start != NULL) {
        m->buffer = start + 4;
        m->mul_enabled = 1;
        m->state = states[multiplier_mul];
    } else {
        m->state = states[multiplier_end];
    }
}

void next_state_mul(struct multiplier* m)
{
    char *mul = strstr(m->buffer, "mul");
    char *stop = strstr(m->buffer, "don't()");
    if (mul != NULL && (stop == NULL || mul < stop)) {
        m->buffer = mul + 3;
        m->state = states[multiplier_parentheses_open];
    } else if (stop != NULL) {
        m->buffer = stop + 7;
        m->mul_enabled = 0;
        m->state = states[multiplier_start];
    } else {
        m->state = states[multiplier_end];
    }
}

void next_state_parentheses_open(struct multiplier* m)
{
    if (*(m->buffer)) {
        m->buffer++;
        m->number_one = 0;
        m->number_two = 0;
        m->state = states[multiplier_number_one];
    } else {
        m->state = states[multiplier_mul];
    }
}

void next_state_number_one(struct multiplier* m)
{
    int digit_count = 0;

    while(isdigit(*(m->buffer))) {
        m->number_one = m->number_one * 10 + (*(m->buffer) - '0');
        m->buffer++;
        digit_count++;
    }

    if (digit_count > 0 && digit_count <= 3) {
        m->state = states[multiplier_comma];
    } else {
        m->state = states[multiplier_mul];
    }
}

void next_state_comma(struct multiplier* m)
{
    if (*(m->buffer) == ',') {
        m->buffer++;
        m->state = states[multiplier_number_two];
    } else {
        m->state = states[multiplier_mul];
    }
}

void next_state_number_two(struct multiplier* m)
{
    int digit_count = 0;

    while(isdigit(*(m->buffer))) {
        m->number_two = m->number_two * 10 + (*(m->buffer) - '0');
        m->buffer++;
        digit_count++;
    }

    if (digit_count > 0 && digit_count <= 3) {
        m->state = states[multiplier_parentheses_close];
    } else {
        m->state = states[multiplier_mul];
    }
}

void next_state_parentheses_close(struct multiplier* m)
{
    if (*(m->buffer) == ')') {
        m->buffer++;
        m->state = states[multiplier_complete];
    } else {
        m->state = states[multiplier_mul];
    }
}

void next_state_complete(struct multiplier* m)
{
    m->total_multiplications += m->number_one * m->number_two;
    m->state = states[multiplier_mul];
}

int main(void)
{
    char buffer[BUFSIZ];

    int mul_enabled = 1;
    int total_multiplications = 0;

    while (fgets(buffer, BUFSIZ, stdin) != NULL) {
        struct multiplier m;
        m.state = mul_enabled? states[multiplier_mul] : states[multiplier_start];
        m.buffer = buffer;
        m.mul_enabled = mul_enabled;
        m.total_multiplications = 0;

        while (m.state.state != multiplier_end) {
            m.state.next_state(&m);
        }

        total_multiplications += m.total_multiplications;
        mul_enabled = m.mul_enabled;
    }
    
    printf("Total of multiplications = %d\n", total_multiplications);
}