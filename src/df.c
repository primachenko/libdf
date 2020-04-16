#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "df.h"
#include "debug.h"

static FPN buf_left_shift_pop(FPN    * buffer,
                              size_t   size)
{
    FPN val = buffer[0];
    for (int i = 0; i < size - 1; ++i)
    {
        buffer[i] = buffer[i + 1];
    }
    buffer[size - 1] = 0.0;
#ifdef SELF_EXCITATION_DETECTION
#ifdef USE_FLOAT/
    if (FLT_MAX == val)
#else/
    if (DBL_MAX == val)
#endif /* USE_FLOAT */
    {
        DF_DBG("[WARNING] filter self-excitation detected");
    }
#endif /* SELF_EXCITATION_DETECTION */
    return val;
}

FPN filter_apply(filter_t * f,
                 FPN        x)
{
    for (int i = 0; i < f->b_size; i++)
    {
        f->buf[i] += x * f->b[i];
    }

    for (int i = 1; i < f->a_size; i++)
    {
        f->buf[i] -= f->buf[0] * f->a[i];
    }

    return buf_left_shift_pop(f->buf, f->buf_size);
}

void filter_apply_on_data(filter_t     * f,
                          const FPN    * x,
                                FPN    * y,
                          const size_t   size)
{
    DF_DBG("Input data array length %lu", size);
    for (int n = 0; n < size; ++n)
    {
        y[n] = filter_apply(f, x[n]);
    }
    DF_DBG("Filter data array done");
}

static void normalize_coefficients(filter_t * f)
{
    if ((!isinf(f->a[0])) && (!isnan(f->a[0])) &&
        (!(f->a[0] == 0.0)) && (f->a[0] != 1.0))
    {
        for (int k = 0; k < f->b_size; k++)
        {
            f->b[k] /= f->a[0];
        }

        for (int k = 1; k < f->a_size; k++)
        {
            f->a[k] /= f->a[0];
        }

        f->a[0] = 1.0;
    }
}

static filter_t * filter_create(const FPN   * a,
                                const size_t  a_size,
                                const FPN   * b,
                                const size_t  b_size)
{
    filter_t * f = calloc(1, sizeof(filter_t));
    if (!f)
    {
        DF_ERR("calloc failed");
        return NULL;
    }

    f->a_size = a_size;
    f->b_size = b_size;
    f->buf_size = a_size > b_size ? a_size : b_size;

    f->a = calloc(f->a_size, sizeof(FPN));
    if (!f->a)
    {
        DF_ERR("calloc failed");
        filter_destroy(f);
        return NULL;
    }
    memcpy(f->a, a, f->a_size * sizeof(FPN));

    f->b = calloc(f->b_size, sizeof(FPN));
    if (!f->a)
    {
        DF_ERR("calloc failed");
        filter_destroy(f);
        return NULL;
    }
    memcpy(f->b, b, f->b_size * sizeof(FPN));

    normalize_coefficients(f);

#ifdef DF_DEBUG
    for (int i = 0; i < a_size; ++i)
    {
        DF_DBG("a[%d]=%30.30lf", i, f->a[i]);
    }

    for (int i = 0; i < b_size; ++i)
    {
        DF_DBG("b[%d]=%30.30lf", i, f->b[i]);
    }
#endif /* DF_DEBUG */
    f->buf = calloc(f->buf_size, sizeof(FPN));
    if (!f->buf)
    {
        DF_ERR("calloc failed");
        filter_destroy(f);
        return NULL;
    }

    return f;
}

filter_t * filter_iir_create(const FPN    * a,
                             const size_t   a_size,
                             const FPN    * b,
                             const size_t   b_size)
{
    return filter_create(a, a_size, b, b_size);
}

filter_t * filter_fir_create(const FPN    * b,
                             const size_t   b_size)
{
    FPN a_in = 1.0;
    return filter_create(&a_in, 1, b, b_size);
}

static void safe_free(void * ptr)
{
    if (ptr)
    {
        free(ptr);
        ptr = NULL;
    }
}

void filter_destroy(filter_t * f)
{
    safe_free(f->a);
    safe_free(f->b);
    safe_free(f->buf);
    safe_free(f);
}