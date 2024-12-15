#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef long long (*operator)(long long, long long);

struct operator {
    char *name;
    operator op;
};

long long add_operator(long long a, long long b) {
    return a + b;
}

long long multiply_operator(long long a, long long b) {
    return a * b;
}

size_t operators_size = 2;
struct operator operators[] = {
    {"+", add_operator},
    {"*", multiply_operator}
};

struct equation {
    long long test_value;
    long long *numbers;
    int valid;
    size_t numbers_size;
    size_t numbers_capacity;
    struct operator** operators;
    size_t operators_size;
    size_t operators_capacity;
};

struct equation* equation_create(long long test_value) {
    struct equation *equation = malloc(sizeof(struct equation));
    equation->test_value = test_value;
    equation->numbers = NULL;
    equation->numbers_size = 0;
    equation->numbers_capacity = 0;
    equation->valid = 0;
    equation->operators = NULL;
    equation->operators_size = 0;
    equation->operators_capacity = 0;
    return equation;
}

void equation_print_input(struct equation *equation) {
    printf("%lld:", equation->test_value);
    for (size_t i = 0; i < equation->numbers_size; i++) {
        printf(" %lld", equation->numbers[i]);
    }
    printf("\n");
}

void equation_print_result(struct equation *equation) {
    printf("%lld:", equation->test_value);
    for (size_t i = 0; i < equation->numbers_size; i++) {
        printf(" %lld", equation->numbers[i]);
        if (i < equation->operators_size) {
            printf(" %s", equation->operators[i]->name);
        }
    }
    printf("\n");
}

void equation_destroy(struct equation *equation) {
    free(equation->numbers);
    free(equation->operators);
    free(equation);
}

void equation_add_number(struct equation *equation, long long number) {
    if (equation->numbers_size == equation->numbers_capacity) {
        equation->numbers_capacity = equation->numbers_capacity == 0 ? 1 : equation->numbers_capacity * 2;
        equation->numbers = realloc(equation->numbers, equation->numbers_capacity * sizeof(long long));
    }
    equation->numbers[equation->numbers_size++] = number;
}

void equation_push_operator(struct equation *equation, struct operator* op) {
    if (equation->operators_size == equation->operators_capacity) {
        equation->operators_capacity = equation->operators_capacity == 0 ? 1 : equation->operators_capacity * 2;
        equation->operators = realloc(equation->operators, equation->operators_capacity * sizeof(operator));
    }
    equation->operators[equation->operators_size++] = op;
}

void equation_pop_operator(struct equation *equation) {
    if (equation->operators_size > 0) {
        equation->operators_size--;
    }
}

void equation_set_valid(struct equation *equation, long long result) {
    equation->valid = result == equation->test_value;
}

long long equation_calculate(struct equation *equation) {
    // printf ("Testing equation: ");
    // equation_print_result(equation);
    if (equation->numbers_size == 0) {
        return 0;
    }
    long long result = equation->numbers[0];
    for (size_t i = 0; i < equation->operators_size; i++) {
        // printf("Applying operator: %s\n", equation->operators[i]->name);
        result = equation->operators[i]->op(result, equation->numbers[i + 1]);
        // printf("Result: %lld\n", result);
    }
    return result;
}

int equation_is_valid(struct equation *equation) {
    return equation->valid;
}

void equation_solve_index(struct equation *equation, size_t index) {
    if (index == equation->numbers_size - 1) {
        long long result = equation_calculate(equation);
        equation_set_valid(equation, result);
    } else {
        for (size_t i = 0; !equation_is_valid(equation) && i < operators_size; i++) {
            equation_push_operator(equation, &operators[i]);
            equation_solve_index(equation, index + 1);
            equation_pop_operator(equation);
        }
    }
}

void equation_solve(struct equation *equation) {
    // printf("Solving equation: ");
    // equation_print_input(equation);
    equation_solve_index(equation, 0);
}

struct equations {
    struct equation** equations;
    size_t equations_size;
    size_t equations_capacity;
};

struct equations* equations_create() {
    struct equations* equations = malloc(sizeof(struct equations));
    equations->equations = NULL;
    equations->equations_size = 0;
    equations->equations_capacity = 0;
    return equations;
}

void equations_destroy(struct equations *equations) {
    for (size_t i = 0; i < equations->equations_size; i++) {
        equation_destroy(equations->equations[i]);
    }
    free(equations->equations);
    free(equations);
}

void equations_add(struct equations *equations, struct equation *equation) {
    if (equations->equations_size == equations->equations_capacity) {
        equations->equations_capacity = equations->equations_capacity == 0 ? 1 : equations->equations_capacity * 2;
        equations->equations = realloc(equations->equations, equations->equations_capacity * sizeof(struct equation*));
    }
    equations->equations[equations->equations_size++] = equation;
}

void equations_read_from_stdin(struct equations *equations) {
    char buffer[BUFSIZ];
    while(fgets(buffer, BUFSIZ, stdin) != NULL) {
        char *token = strtok(buffer, ":");
        struct equation *equation = equation_create(atoll(buffer));
        while ((token = strtok(NULL, " ")) != NULL) {
            equation_add_number(equation, atoll(token));
        }
        equations_add(equations, equation);
    }
}

void equations_print_input(struct equations *equations) {
    for (size_t i = 0; i < equations->equations_size; i++) {
        equation_print_input(equations->equations[i]);
    }
}

void equations_print_valid(struct equations *equations) {
    for (size_t i = 0; i < equations->equations_size; i++) {
        struct equation *equation = equations->equations[i];
        if (equation_is_valid(equation)) {
            equation_print_result(equation);
        }
    }
}

void equations_solve(struct equations *equations) {
    for (size_t i = 0; i < equations->equations_size; i++) {
        equation_solve(equations->equations[i]);
    }
}

long equations_calculate_total_calibration_result(struct equations *equations) {
    long long total_calibration_result = 0;
    for (size_t i = 0; i < equations->equations_size; i++) {
        if (equation_is_valid(equations->equations[i])) {
            total_calibration_result += equations->equations[i]->test_value;
        }
    }
    return total_calibration_result;
}

int main() {
    long long total_calibration_result = 0;

    struct equations *equations = equations_create();

    equations_read_from_stdin(equations);
    equations_print_input(equations);

    printf("Valid equations:\n");
    equations_solve(equations);
    equations_print_valid(equations);
    total_calibration_result = equations_calculate_total_calibration_result(equations);

    printf("Total calibration value: %lld\n", total_calibration_result);

    equations_destroy(equations);

    return 0;
}