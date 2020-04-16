#ifndef DF_H
#define DF_H

/* Using single-precision numbers speeds up execution and reduces memory consumption */
#ifdef USE_FLOAT
typedef float FPN;
#else
typedef double FPN;
#endif /* USE_FLOAT */

/* Get warnings in case of filter self-excitation */
#ifdef SELF_EXCITATION_DETECTION
#define SELF_EXCITATION_DETECT
#endif /* SELF_EXCITATION_DETECTION */

typedef struct
{
    FPN * a;
    int   a_size;
    FPN * b;
    int   b_size;
    FPN * buf;
    int   buf_size;
} filter_t;

filter_t * filter_iir_create(const FPN    * a,
                             const size_t   a_size,
                             const FPN    * b,
                             const size_t   b_size);

filter_t * filter_fir_create(const FPN    * b,
                             const size_t   b_size);

FPN filter_apply(filter_t * f,
                 FPN        x);

void filter_apply_on_data(filter_t     * f,
                          const FPN    * x,
                                FPN    * y,
                          const size_t   size);

void filter_destroy(filter_t * filter);

/*
Example:

const int order = 1;
double b[order] = {1.0};
filter_t * f = filter_fir_create(&b[0], order);
filter_apply_on_data(f, data_in, data_out, data_size);
filter_destroy(f);
*/

#endif /* DF_H */