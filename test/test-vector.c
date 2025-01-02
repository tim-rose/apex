/*
 * VECTOR.C --Unit tests for the vector module.
 *
 * Contents:
 * compare_long() --CompareProc for long data.
 * visit_long()   --VisitProc for long data.
 * test_init()    --Run tests relating to new_vector/free_vector().
 * test_insert()  --Run tests relating to Insert/Add/delete functions.
 * test_search()  --Run tests relating to Search functions.
 * test_visit()   --Run tests relating to Visit functions.
 */
#include <stdbool.h>
#include <string.h>

#include <apex/tap.h>
#include <apex.h>
#include <apex/vector.h>
#include <apex/compare.h>

static int visit_long(const void *v1, const void *v2);
static void test_init(void);
static void test_insert(void);
static void test_search(void);
static void test_visit(void);

static char cbuf[] = { '0', '1', '2', '3', '4' };
static short sbuf[] = { 0, 1, 2, 3, 4 };
static long lbuf[] = { 0, 2, 4, 6, 8 };
static double dbuf[] = { 0.0, 1.0, 2.0, 3.0, 4.0 };

int main(void)
{
    plan_tests(27);
    test_init();
    test_insert();
    test_search();
    test_visit();
    return exit_status();
}

/*
 * visit_long() --VisitProc for long data.
 */
static int visit_long(const void *v1, const void *v2)
{
    long *l1 = (long *) v1;
    long *l2 = (long *) v2;

    return *l1 == *l2;
}

/*
 * test_init() --Run tests relating to new_vector/free_vector().
 *
 * Note: a number of the unit tests do a long-winded loop to compare
 * results, where memcmp() would arguably be better/simpler.  This is
 * intentional, because we are also implicitly testing the alignment
 * behaviour by accessing the elements directly, not as a (void *).
 */
static void test_init(void)
{
    char *cv;
    short *sv;
    long *lv;
    double *dv;
    VectorInfo vinfo;
    size_t i, status;

    /*
     * char-size tests
     */
    cv = new_vector(sizeof(*cv), NEL(cbuf), cbuf);

    for (status = 1, i = 0; i < NEL(cbuf); ++i)
    {
        if (cv[i] != cbuf[i])
        {                              /* mismatch */
            status = 0;
        }
    }
    ok(status, "new_vector() (char) initialisation");

    ok(vector_info(cv, &vinfo) == &vinfo &&
       vinfo.el_size == sizeof(*cv) &&
       vinfo.n_used == NEL(cbuf) &&
       vinfo.n_used <= vinfo.n_el, "vector_info() (char) properties");

    free_vector(cv);

    /*
     * short-size tests
     */
    sv = new_vector(sizeof(*sv), NEL(sbuf), sbuf);
    for (status = 1, i = 0; i < NEL(sbuf); ++i)
    {
        if (sv[i] != sbuf[i])
        {                              /* mismatch */
            status = 0;
        }
    }
    ok(status, "new_vector() (short) initialisation");

    ok(vector_info(sv, &vinfo) == &vinfo &&
       vinfo.el_size == sizeof(*sv) &&
       vinfo.n_used == NEL(sbuf) &&
       vinfo.n_used <= vinfo.n_el, "vector_info() (short) properties");
    free_vector(sv);

    /*
     * long-size tests
     */
    lv = new_vector(sizeof(*lv), NEL(lbuf), lbuf);
    for (status = 1, i = 0; i < NEL(lbuf); ++i)
    {
        if (lv[i] != lbuf[i])
        {                              /* mismatch */
            status = 0;
        }
    }
    ok(status, "new_vector() (long) initialisation");

    ok(vector_info(lv, &vinfo) == &vinfo &&
       vinfo.el_size == sizeof(*lv) &&
       vinfo.n_used == NEL(lbuf) &&
       vinfo.n_used <= vinfo.n_el, "vector_info() (long) properties");
    free_vector(lv);

    /*
     * double-size tests
     */
    dv = new_vector(sizeof(*dv), NEL(dbuf), dbuf);
    for (status = 1, i = 0; i < NEL(dbuf); ++i)
    {
        if (dv[i] != dbuf[i])
        {                              /* mismatch */
            status = 0;
        }
    }
    ok(status, "new_vector() (double) initialisation");

    ok(vector_info(dv, &vinfo) == &vinfo &&
       vinfo.el_size == sizeof(*dv) &&
       vinfo.n_used == NEL(dbuf) &&
       vinfo.n_used <= vinfo.n_el, "vector_info() (double) properties");
    free_vector(dv);

    ok(vector_len(NULL) == -1, "vector_len() failure-mode");
}

/*
 * test_insert() --Run tests relating to Insert/Add/delete functions.
 *
 * Remarks:
 * All of these tests are performed on long arrays only.
 */
static void test_insert(void)
{
    long *lv;
    size_t i;
    size_t n = NEL(lbuf);
    VectorInfo vinfo;
    static long lresult[] = {          /* reference data */
        0, 2, 4, 6, 8,
        0, 2, 4, 6, 8,
        0, 2, 4, 6, 8,
        0, 2, 4, 6, 8,
        0, 2, 4, 6, 8,
        0, 2, 4, 6, 8
    };

    lv = new_vector(sizeof(*lv), n, lbuf);
    for (i = 0; i < 5; ++i)
    {
        lv = vector_add(lv, n, lbuf);
    }
    ok(memcmp(lv, lresult, sizeof(lresult)) == 0 &&
       vector_info(lv, &vinfo)->n_used == 30, "vector_add()");
    free_vector(lv);

    lv = new_vector(sizeof(*lv), n, lbuf);
    for (i = 0; i < 5; ++i)
    {
        lv = vector_insert(lv, i * n, n, lbuf);
    }
    ok(memcmp(lv, lresult, sizeof(lresult)) == 0 &&
       vector_info(lv, &vinfo)->n_used == 30, "vector_insert() at end");
    free_vector(lv);

    lv = new_vector(sizeof(*lv), n, lbuf);
    for (i = 0; i < 5; ++i)
    {
        lv = vector_insert(lv, 0, n, lbuf);
    }
    ok(memcmp(lv, lresult, sizeof(lresult)) == 0 &&
       vector_info(lv, &vinfo)->n_used == 30, "vector_insert() at beginning");

    lv[25] = 10;
    lv[26] = 20;
    lv[27] = 30;
    lv[28] = 40;
    lv[29] = 50;

    for (i = 0; i < 5; ++i)
    {
        lv = vector_delete(lv, 0, n);
    }
    ok(lv[0] == 10 &&
       lv[1] == 20 &&
       lv[2] == 30 &&
       lv[3] == 40 &&
       lv[4] == 50 &&
       vector_info(lv, &vinfo)->n_used == 5, "vector_delete()");
    free_vector(lv);
}

/*
 * test_search() --Run tests relating to Search functions.
 *
 * Remarks:
 * All of these tests are performed on long arrays only.
 */
static void test_search(void)
{
    long *lv;
    long key;
    size_t n = NEL(lbuf);
    bool status;

    lv = new_vector(sizeof(*lv), n, lbuf);

    for (int i = 0; i < (int) n; ++i)
    {
        key = (long) i *2;
        ok(search_vector(lv, &key, long_cmp, &status) == i &&
           status == true, "Successful search_vector(%d)", i);
    }

    key = -1;                          /* insert at beginning */
    ok(search_vector(lv, &key, long_cmp, &status) == 0 &&
       status == false, "Unsuccesssful search_vector(1)");
    key = 10;                          /* insert after end */
    ok(search_vector(lv, &key, long_cmp, &status) == (int) n &&
       status == false, "Unsuccesssful search_vector(2)");
    key = 5;                           /* insert in middle */
    ok(search_vector(lv, &key, long_cmp, &status) == 3 &&
       status == false, "Unsuccesssful search_vector(3)");
    free_vector(lv);
}


/*
 * test_visit() --Run tests relating to Visit functions.
 *
 * Remarks:
 * All of these tests are performed on long arrays only.
 */
static void test_visit(void)
{
    long *lv;
    long key;
    long *result;
    size_t i;
    size_t n = NEL(lbuf);

    lv = new_vector(sizeof(*lv), n, lbuf);

    for (i = 0; i < n; ++i)
    {
        key = (long) i *2;
        result = visit_vector(lv, (VisitProc) visit_long, (void *) &key);
        ok(result != NULL
           && *result == key, "Successful visit_vector(%d)", i);
    }

    key = -1;
    result = visit_vector(lv, (VisitProc) visit_long, (void *) &key);
    ok(result == NULL, "Unsuccessful visit_vector(%z)", i);
    free_vector(lv);
}
