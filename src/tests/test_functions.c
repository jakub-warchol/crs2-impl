#include "test_functions.h"

#include <math.h>
#include <stdlib.h>

double goldstein_price_function(const int n, double *args)
{
    if(n != 2) {
        return 0;
    }

    double x1 = args[0];
    double x2 = args[1];

    return (1 + pow((x1 + x2 + 1),2) * (19 - 14*x1 + 3*pow(x1,2) - 14*x2 + 6*x1*x2 + 3*pow(x2,2))) * (30 + pow((2*x1 - 3*x2),2) * (18 - 32*x1 + 12*pow(x1,2) + 48*x2 - 36*x1*x2 + 27*pow(x2,2)));
}

int goldstein_price_constraint(int argNo, double value)
{
    if(argNo == 0) {
        if(value >= -2) {
            return 0;
        } else {
            return -1;
        }
    } else if(argNo == 1) {
        if(value <= 2) {
            return 0;
        } else {
            return 1;
        }
    }

    return -1;
}

double levy_function(const int n, double *args)
{
    if(n != 2) {
        return 0;
    }

    double x1 = args[0];
    double x2 = args[1];

    return pow(sin(3*M_PI*x1),2) + pow((x1-1),2) * pow(1+(sin(3*M_PI*x2)),2) + pow((x2-1),2) * pow(1+(sin(2*M_PI*x2)),2);
}

int levy_constraint(int argNo, double value)
{
    //NOTE: Levy function has the same constraint for every arguments

    if(value < -10) {
        return -1;
    } else if(value > 10) {
        return 1;
    }

    return 0;
}

double eggholder_function(const int n, double *args)
{
    if(n != 2) {
        return 0;
    }

    double x1 = args[0];
    double x2 = args[1];

    return -(x2+47) * sin(sqrt(fabs(x2+x1/2+47))) - x1 * sin(sqrt(fabs(x1-(x2+47))));
}

int eggholder_constraint(int argNo, double value)
{
    //NOTE: Eggholder function has the same constraint for every arguments

    if(value < -512) {
        return -1;
    } else if(value > 512) {
        return 1;
    }

    return 0;
}

double drop_wave_function(const int n, double *args)
{
    if(n != 2) {
        return 0;
    }

    double x1 = args[0];
    double x2 = args[1];

    return -(1 + cos(12*sqrt(pow(x1,2)+pow(x2,2)))) / (0.5*(pow(x1,2)+pow(x2,2)) + 2);
}

int drop_wave_constraint(int argNo, double value)
{
    //NOTE: Drop_wave function has the same constraint for every arguments

    if(value < -512) {
        return -1;
    } else if(value > 512) {
        return 1;
    }

    return 0;
}

double mccormick_function(const int n, double *args)
{
    if(n != 2) {
        return 0;
    }

    double x1 = args[0];
    double x2 = args[1];

    return sin(x1 + x2) + pow((x1 - x2),2) - (1.5*x1) + 2.5*x2 + 1;
}

int mccormick_constraint(int argNo, double value)
{
    if(argNo == 0) {
        if(value < -1.5) {
            return -1;
        } else if(value > 4) {
            return 1;
        } else {
            return 0;
        }
    } else if(argNo == 1) {
        if(value < -3) {
            return -1;
        } else if(value > 4){
            return 1;
        } else {
            return 0;
        }
    }

    return -1;
}
