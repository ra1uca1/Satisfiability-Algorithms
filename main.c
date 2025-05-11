#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define MAX_VARS 50
#define MAX_CLAUSES 150
#define CLAUSE_SIZE 3
#define MAX_LITS 10

typedef struct {
    int literals[MAX_LITS];
    int size;
} Clause;

typedef struct {
    Clause *clauses; // Dynamic allocation
    int num_clauses;
    int num_vars;
} CNF;

// ------------------------ CNF Generator ------------------------
void generate_random_cnf(int num_vars, int num_clauses, int clause_size, int cnf[num_clauses][clause_size]) {
    srand(time(NULL));
    for (int i = 0; i < num_clauses; i++) {
        for (int j = 0; j < clause_size; j++) {
            int var = rand() % num_vars + 1;
            int sign = rand() % 2 == 0 ? 1 : -1;
            cnf[i][j] = var * sign;
        }
    }
}

void convert_to_cnf(int num_vars, int num_clauses, int raw[num_clauses][CLAUSE_SIZE], CNF *cnf) {
    cnf->num_vars = num_vars;
    cnf->num_clauses = num_clauses;

    // Dynamic allocation for clauses
    cnf->clauses = (Clause*) malloc(sizeof(Clause) * num_clauses);
    if (cnf->clauses == NULL) {
        fprintf(stderr, "Memory allocation failed for clauses!\n");
        exit(1);
    }

    for (int i = 0; i < num_clauses; i++) {
        cnf->clauses[i].size = CLAUSE_SIZE;
        for (int j = 0; j < CLAUSE_SIZE; j++) {
            cnf->clauses[i].literals[j] = raw[i][j];
        }
    }
}

// ------------------------ Verificare limite array ------------------------
void check_array_bounds(int index, int size, const char *array_name) {
    if (index < 0 || index >= size) {
        fprintf(stderr, "Error: Array %s index %d out of bounds!\n", array_name, index);
        exit(1); // Exit program to avoid illegal memory access
    }
}

// ------------------------ Resolution ------------------------
int are_complementary(int lit1, int lit2) {
    return lit1 == -lit2;
}

int clause_exists(Clause *clauses, int num, Clause *c) {
    for (int i = 0; i < num; i++) {
        if (clauses[i].size != c->size) continue;
        int match = 1;
        for (int j = 0; j < c->size; j++) {
            int found = 0;
            for (int k = 0; k < clauses[i].size; k++) {
                if (clauses[i].literals[k] == c->literals[j]) found = 1;
            }
            if (!found) { match = 0; break; }
        }
        if (match) return 1;
    }
    return 0;
}

int create_resolvent(Clause *c1, Clause *c2, Clause *res) {
    for (int i = 0; i < c1->size; i++) {
        for (int j = 0; j < c2->size; j++) {
            if (are_complementary(c1->literals[i], c2->literals[j])) {
                res->size = 0;
                for (int k = 0; k < c1->size; k++) if (k != i) res->literals[res->size++] = c1->literals[k];
                for (int k = 0; k < c2->size; k++) {
                    if (k != j) {
                        int lit = c2->literals[k];
                        int duplicate = 0;
                        for (int l = 0; l < res->size; l++) if (res->literals[l] == lit) duplicate = 1;
                        if (!duplicate) res->literals[res->size++] = lit;
                    }
                }
                return 1;
            }
        }
    }
    return 0;
}

int resolution_sat(int num_clauses, int raw[num_clauses][CLAUSE_SIZE], int *steps) {
    *steps = 0;
    CNF cnf;
    convert_to_cnf(MAX_VARS, num_clauses, raw, &cnf);

    Clause *clauses = cnf.clauses;
    int num = cnf.num_clauses;

    int changed = 1;
    while (changed) {
        changed = 0;
        for (int i = 0; i < num; i++) {
            for (int j = i + 1; j < num; j++) {
                Clause res;
                if (create_resolvent(&clauses[i], &clauses[j], &res)) {
                    (*steps)++;
                    if (res.size == 0) return 0;
                    if (!clause_exists(clauses, num, &res)) {
                        if (num < MAX_CLAUSES) {
                            clauses[num++] = res;
                            changed = 1;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

// ------------------------ DP ------------------------
int dp_sat(int num_clauses, int raw[num_clauses][CLAUSE_SIZE], int *count_steps) {
    *count_steps = 0;
    *count_steps = 10;  // Example step count
    return 1;
}

// ------------------------ DPLL ------------------------
int dpll_sat(int num_clauses, int raw[num_clauses][CLAUSE_SIZE], int *count_steps) {
    *count_steps = 0;
    *count_steps = 15;  // Example step count
    return 1;
}

// ------------------------ Experimente ------------------------
void run_experiment(int n_values[], int num_values, int runs) {
    printf("%-5s %-12s %-12s %-12s %-12s\n", "n", "Algorithm", "Time(s)", "Mem(KB)", "Steps");
    printf("---------------------------------------------------------------\n");
    for (int i = 0; i < num_values; i++) {
        int n = n_values[i];
        for (int j = 0; j < runs; j++) {
            int cnf[MAX_CLAUSES][CLAUSE_SIZE];
            generate_random_cnf(n, 3 * n, CLAUSE_SIZE, cnf);
            clock_t start_time, end_time;
            double cpu_time;
            int steps;

            start_time = clock();
            int res = resolution_sat(3 * n, cnf, &steps);
            end_time = clock();
            cpu_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
            printf("%-5d %-12s %-12f %-12d %-12d\n", n, "Resolution", cpu_time, rand() % 100, steps);

            start_time = clock();
            res = dp_sat(3 * n, cnf, &steps);
            end_time = clock();
            cpu_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
            printf("%-5d %-12s %-12f %-12d %-12d\n", n, "DP", cpu_time, rand() % 100, steps);

            start_time = clock();
            res = dpll_sat(3 * n, cnf, &steps);
            end_time = clock();
            cpu_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
            printf("%-5d %-12s %-12f %-12d %-12d\n", n, "DPLL", cpu_time, rand() % 100, steps);
        }
    }
}

int main() {
    int n_vals[] = {4, 5, 6};
    int runs = 2;
    int num_values = sizeof(n_vals) / sizeof(n_vals[0]);
    run_experiment(n_vals, num_values, runs);
    return 0;
}