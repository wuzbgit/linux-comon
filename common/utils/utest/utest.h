#ifndef __UTEST_H__
#define __UTEST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

/**
 * utest_error
 *
 * @brief Test result.
 *
 * @member UTEST_PASSED Test success.
 * @member UTEST_FAILED Test failed.
 * @member UTEST_PASSED Test skipped.
 *
*/
enum utest_error
{
    UTEST_PASSED  = 0,
    UTEST_FAILED  = 1,
    UTEST_SKIPPED = 2
};
typedef enum utest_error utest_err_e;

/**
 * utest
 *
 * @brief utest data structure.
 *
 * @member error      Error number from enum `utest_error`.
 * @member passed_num Total number of tests passed.
 * @member failed_num Total number of tests failed.
 *
*/
struct utest
{
    utest_err_e error;
    uint32_t passed_num;
    uint32_t failed_num;
};
typedef struct utest *utest_t;

/**
 * utest_tc_export
 *
 * @brief utest testcase data structure.
 *        Will export the data to `UtestTcTab` section in flash.
 *
 * @member name        Testcase name.
 * @member run_timeout Testcase maximum test time (Time unit: seconds).
 * @member init        Necessary initialization before executing the test case function.
 * @member tc          Total number of tests failed.
 * @member cleanup     Total number of tests failed.
 *
*/
struct utest_tc_export {
    const char  *module;
    const char  *name;
    uint32_t     run_timeout;
    int          process_fork;
    void         *priv;
    int   (*init)(struct utest_tc_export *);
    void       (*tc)(struct utest_tc_export *);
    void       (*fork_assert)(struct utest_tc_export *,int result,int status);
    int   (*cleanup)(struct utest_tc_export *);
};
typedef struct utest_tc_export *utest_tc_export_t;

/**
 * test_unit_func
 *
 * @brief Unit test handler function pointer.
 *
*/
typedef void (*test_unit_func)(void);

/* No need for the user to use this function directly */
void utest_assert(int value, const char *file, int line, const char *func, const char *msg);

/* No need for the user to use this function directly */
void utest_assert_string(const char *a, const char *b, bool equal, const char *file, int line, const char *func, const char *msg);
void utest_assert_buf(const char *a, const char *b, size_t sz, bool equal, const char *file, int line, const char *func, const char *msg);

/* No need for the user to use this macro directly */
#define __utest_assert(value, msg) utest_assert(value, __FILE__, __LINE__, __func__, msg)

#define uassert_true(value)      __utest_assert(value, "(" #value ") is false")
#define uassert_false(value)     __utest_assert(!(value), "(" #value ") is true")
#define uassert_null(value)      __utest_assert((const char *)(value) == RT_NULL, "(" #value ") is not null")
#define uassert_not_null(value)  __utest_assert((const char *)(value) != RT_NULL, "(" #value ") is null")

#define uassert_int_equal(a, b)      __utest_assert((a) == (b), "(" #a ") not equal to (" #b ")")
#define uassert_int_not_equal(a, b)  __utest_assert((a) != (b), "(" #a ") equal to (" #b ")")

#define uassert_str_equal(a, b)      utest_assert_string((const char*)(a), (const char*)(b), RT_TRUE, __FILE__, __LINE__, __func__, "string not equal")
#define uassert_str_not_equal(a, b)  utest_assert_string((const char*)(a), (const char*)(b), RT_FALSE, __FILE__, __LINE__, __func__, "string equal")

#define uassert_buf_equal(a, b, sz)      utest_assert_buf((const char*)(a), (const char*)(b), (sz), RT_TRUE, __FILE__, __LINE__, __func__, "buf not equal")
#define uassert_buf_not_equal(a, b, sz)  utest_assert_buf((const char*)(a), (const char*)(b), (sz), RT_FALSE, __FILE__, __LINE__, __func__, "buf equal")

#define uassert_in_range(value, min, max)     __utest_assert(((value >= min) && (value <= max)), "(" #value ") not in range("#min","#max")")
#define uassert_not_in_range(value, min, max) __utest_assert(!((value >= min) && (value <= max)), "(" #value ") in range("#min","#max")")
/**
 * utest_handle_get
 *
 * @brief Get the utest data structure handle.
 *        No need for the user to call this function directly
 *
 * @param void
 *
 * @return utest_t type. (struct utest *)
 *
*/
utest_t utest_handle_get(void);

int utest_run(struct utest_tc_export *tc,unsigned loop_count);
int utest_statistic();
int utest_init(const char *report_path,const char *msg);
void utest_deinit(void);
int waitpid_timeout(pid_t pid,int mseconds,int *status);
/**
 * UTEST_NAME_MAX_LEN
 *
 * @brief Testcase name maximum length.
 *
*/
#define UTEST_NAME_MAX_LEN (128)

/**
 * UTEST_TC_EXPORT
 *
 * @brief Export testcase function to `UtestTcTab` section in flash.
 *        Used in application layer.
 *
 * @param testcase The testcase function.
 * @param name     The testcase name.
 * @param init     The initialization function of the test case.
 * @param cleanup  The cleanup function of the test case.
 * @param timeout  Testcase maximum test time (Time unit: seconds).
 *
 * @return None
 *
*/

#ifdef __cplusplus
}
#endif

#endif /* __UTEST_H__ */
