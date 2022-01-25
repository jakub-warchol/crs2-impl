#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

typedef enum function_type_t {
    GoldsteinPrice,
    Levy,
    Eggholder,
    DropWave,
    McCormick,
    Colville,
    Griewank,
} function_type_t;

// evaluated functions
double goldstein_price_function(const int n, double *args);
double levy_function(const int n, double *args);
double eggholder_function(const int n, double *args);
double drop_wave_function(const int n, double *args);
double mccormick_function(const int n, double *args);
double colville_function(const int n, double *args);
double griewank_function(const int n, double *args);

// constraint funtions
int goldstein_price_constraint(int argNo, double value);
int levy_constraint(int argNo, double value);
int eggholder_constraint(int argNo, double value);
int drop_wave_constraint(int argNo, double value);
int mccormick_constraint(int argNo, double value);
int colville_constraint(int argNo, double value);
int griewank_constraint(int argNo, double value);

#endif // TEST_FUNCTIONS_H
