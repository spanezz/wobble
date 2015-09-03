#ifndef WOBBLE_TESTS_H
#define WOBBLE_TESTS_H

/**
 * @author Enrico Zini <enrico@enricozini.org>, Peter Rockai (mornfall) <me@mornfall.net>
 * @brief Utility functions for the unit tests
 *
 * Copyright (C) 2006--2007  Peter Rockai (mornfall) <me@mornfall.net>
 * Copyright (C) 2003--2013  Enrico Zini <enrico@debian.org>
 */

#include <string>
#include <sstream>
#include <exception>
#include <functional>
#include <vector>

namespace wobble {
namespace tests {
struct Location;
struct LocationInfo;
}
}

/*
 * These global arguments will be shadowed by local variables in functions that
 * implement tests.
 *
 * They are here to act as default root nodes to fulfill method signatures when
 * tests are called from outside other tests.
 */
extern const wobble::tests::Location wobble_test_location;
extern const wobble::tests::LocationInfo wobble_test_location_info;

namespace wobble {
namespace tests {

/**
 * Exception raised when a test assertion fails, normally by
 * Location::fail_test
 */
struct TestFailed : public std::exception
{
    std::string message;

    TestFailed(const std::string& message) : message(message) {}

    const char* what() const noexcept override { return message.c_str(); }
};

#define WOBBLE_TESTS_ALWAYS_THROWS __attribute__ ((noreturn))

/**
 * Trace the source location of a test function, to provide some backtrace when
 * a test fails.
 */
class Location
{
    const Location* parent;
    const wobble::tests::LocationInfo* info;
    const char* file;
    int line;
    const char* args;

    Location(const Location* parent, const wobble::tests::LocationInfo& info, const char* file, int line, const char* args);

public:
    /**
     * Build a "root" location, that is used as a default and as parent for all
     * other locations
     */
    Location();

    /**
     * Build a nested location, one nesting step down from the current location
     */
    Location nest(const wobble::tests::LocationInfo& info, const char* file, int line, const char* args=0) const;

    /// Return the formatted backtrace for this location
    std::string backtrace() const;

    /// Write the formatted backtrace for this location to \a out
    void backtrace(std::ostream& out) const;

    /**
     * Return a message describing a test failure, including the current
     * backtrace
     */
    std::string fail_msg(const std::string& error) const;

    /**
     * Return a message describing a test failure, including the current
     * backtrace
     */
    std::string fail_msg(std::function<void(std::ostream&)> write_error) const;

    /// Raise TestFailed for the given error message
    void fail_test(const std::string& error) const WOBBLE_TESTS_ALWAYS_THROWS;

    /// Raise TestFailed with error message written by the given function
    void fail_test(std::function<void(std::ostream&)> write_error) const WOBBLE_TESTS_ALWAYS_THROWS;
};

/**
 * Add information to the test backtrace for the tests run in the current
 * scope.
 *
 * Example usage:
 * \code
 * test_function(...)
 * {
 *    WOBBLE_TEST_INFO(info);
 *    for (unsigned i = 0; i < 10; ++i)
 *    {
 *       info() << "Iteration #" << i;
 *       ...
 *    }
 * }
 * \endcode
 */
struct LocationInfo : public std::stringstream
{
    LocationInfo() {}

    /**
     * Clear the current information and return the output stream to which new
     * information can be sent
     */
    std::ostream& operator()();
};

/**
 * Argument to pass to a test function to store the information about where it
 * is being run
 */
#define WOBBLE_TEST_LOCPRM wobble::tests::Location wobble_test_location

/**
 * Use this to declare a local variable with the given name that will be
 * picked up by tests as extra local info
 */
#define WOBBLE_TEST_INFO(name) \
    wobble::tests::LocationInfo wobble_test_location_info; \
    wobble::tests::LocationInfo& name = wobble_test_location_info

/**
 * Signature of test functions.
 */
typedef std::function<void(wobble::tests::Location)> test_function;

/// Test function that ensures that the actual value is true
template<typename A>
struct TestTrue
{
    A actual;
    TestTrue(const A& actual) : actual(actual) {}

    void operator()(WOBBLE_TEST_LOCPRM) const
    {
        if (actual) return;
        wobble_test_location.fail_test("actual value is false");
    }
};

/// Test function that ensures that the actual value is false
template<typename A>
struct TestFalse
{
    A actual;
    TestFalse(const A& actual) : actual(actual) {}

    void operator()(WOBBLE_TEST_LOCPRM) const
    {
        if (!actual) return;
        wobble_test_location.fail_test("actual value is true");
    }
};

/**
 * Test function that ensures that the actual value is the same as a reference
 * one
 */
template<typename A, typename E>
struct TestEquals
{
    A actual;
    E expected;
    TestEquals(const A& actual, const E& expected) : actual(actual), expected(expected) {}

    void operator()(WOBBLE_TEST_LOCPRM) const
    {
        if (actual == expected) return;
        std::stringstream ss;
        ss << "value '" << actual << "' is different than the expected '" << expected << "'";
        wobble_test_location.fail_test(ss.str());
    }
};

/**
 * Test function that ensures that the actual value is different than a
 * reference one
 */
template<typename A, typename E>
struct TestDiffers
{
    A actual;
    E expected;
    TestDiffers(const A& actual, const E& expected) : actual(actual), expected(expected) {}

    void operator()(WOBBLE_TEST_LOCPRM) const
    {
        if (actual != expected) return;
        std::stringstream ss;
        ss << "value '" << actual << "' is not different than the expected '" << expected << "'";
        wobble_test_location.fail_test(ss.str());
    }
};

/// Ensure that the actual value is less than the reference value
template<typename A, typename E>
struct TestLt
{
    A actual;
    E expected;
    TestLt(const A& actual, const E& expected) : actual(actual), expected(expected) {}

    void operator()(WOBBLE_TEST_LOCPRM) const
    {
        if (actual < expected) return;
        std::stringstream ss;
        ss << "value '" << actual << "' is not less than the expected '" << expected << "'";
        wobble_test_location.fail_test(ss.str());
    }
};

/// Ensure that the actual value is less or equal than the reference value
template<typename A, typename E>
struct TestLte
{
    A actual;
    E expected;
    TestLte(const A& actual, const E& expected) : actual(actual), expected(expected) {}

    void operator()(WOBBLE_TEST_LOCPRM) const
    {
        if (actual <= expected) return;
        std::stringstream ss;
        ss << "value '" << actual << "' is not less than or equals to the expected '" << expected << "'";
        wobble_test_location.fail_test(ss.str());
    }
};

/// Ensure that the actual value is greather than the reference value
template<typename A, typename E>
struct TestGt
{
    A actual;
    E expected;
    TestGt(const A& actual, const E& expected) : actual(actual), expected(expected) {}

    void operator()(WOBBLE_TEST_LOCPRM) const
    {
        if (actual > expected) return;
        std::stringstream ss;
        ss << "value '" << actual << "' is not greater than the expected '" << expected << "'";
        wobble_test_location.fail_test(ss.str());
    }
};

/// Ensure that the actual value is greather or equal than the reference value
template<typename A, typename E>
struct TestGte
{
    A actual;
    E expected;
    TestGte(const A& actual, const E& expected) : actual(actual), expected(expected) {}

    void operator()(WOBBLE_TEST_LOCPRM) const
    {
        if (actual >= expected) return;
        std::stringstream ss;
        ss << "value '" << actual << "' is not greater than or equals to the expected '" << expected << "'";
        wobble_test_location.fail_test(ss.str());
    }
};

template<class A>
struct Actual
{
    A actual;
    Actual(const A& actual) : actual(actual) {}
    ~Actual() {}

    test_function istrue() const { return TestTrue<A>(actual); }
    test_function isfalse() const { return TestFalse<A>(actual); }
    template<typename E> test_function operator==(const E& expected) const { return TestEquals<A, E>(actual, expected); }
    template<typename E> test_function operator!=(const E& expected) const { return TestDiffers<A, E>(actual, expected); }
    template<typename E> test_function operator<(const E& expected) const { return TestLt<A, E>(actual, expected); }
    template<typename E> test_function operator<=(const E& expected) const { return TestLte<A, E>(actual, expected); }
    template<typename E> test_function operator>(const E& expected) const { return TestGt<A, E>(actual, expected); }
    template<typename E> test_function operator>=(const E& expected) const { return TestGte<A, E>(actual, expected); }
};

struct ActualCString
{
    const char* actual;
    ActualCString(const char* s) : actual(s) {}

    test_function istrue() const { return TestTrue<bool>(actual); }
    test_function isfalse() const { return TestFalse<bool>(actual); }
    test_function operator==(const char* expected) const;
    test_function operator==(const std::string& expected) const;
    test_function operator!=(const char* expected) const;
    test_function operator!=(const std::string& expected) const;
    test_function operator<(const std::string& expected) const;
    test_function operator<=(const std::string& expected) const;
    test_function operator>(const std::string& expected) const;
    test_function operator>=(const std::string& expected) const;
    test_function startswith(const std::string& expected) const;
    test_function endswith(const std::string& expected) const;
    test_function contains(const std::string& expected) const;
    test_function not_contains(const std::string& expected) const;

protected:
    test_function _actual_must_be_set() const;
};

struct ActualStdString : public Actual<std::string>
{
    ActualStdString(const std::string& s) : Actual<std::string>(s) {}

    test_function operator==(const std::string& expected) const { return TestEquals<std::string, std::string>(actual, expected); }
    test_function operator!=(const std::string& expected) const { return TestDiffers<std::string, std::string>(actual, expected); }
    test_function operator<(const std::string& expected) const { return TestLt<std::string, std::string>(actual, expected); }
    test_function operator<=(const std::string& expected) const { return TestLte<std::string, std::string>(actual, expected); }
    test_function operator>(const std::string& expected) const { return TestGt<std::string, std::string>(actual, expected); }
    test_function operator>=(const std::string& expected) const { return TestGte<std::string, std::string>(actual, expected); }
    test_function startswith(const std::string& expected) const;
    test_function endswith(const std::string& expected) const;
    test_function contains(const std::string& expected) const;
    test_function not_contains(const std::string& expected) const;
//    TestRegexp matches(const std::string& regexp) const { return TestRegexp(actual, regexp); }
//    TestFileExists fileexists() const { return TestFileExists(actual); }
};

template<typename A>
inline Actual<A> actual(const A& actual) { return Actual<A>(actual); }
inline ActualCString actual(const char* actual) { return ActualCString(actual); }
inline ActualCString actual(char* actual) { return ActualCString(actual); }
inline ActualStdString actual(const std::string& actual) { return ActualStdString(actual); }

template<typename T>
static inline void _wassert(WOBBLE_TEST_LOCPRM, const T& expr)
{
    expr(wobble_test_location);
}


#define wobble_test_runner(loc, func, ...) \
    do { try { \
        func(loc, ##__VA_ARGS__); \
    } catch (TestFailed) { \
        throw; \
    } catch (std::exception& e) { \
        loc.fail_test(e.what()); \
    } } while(0)

#define wrunchecked(func) \
    do { try { \
        func; \
    } catch (TestFailed) { \
        throw; \
    } catch (std::exception& e) { \
        wobble_test_location.nest(wobble_test_location_info, __FILE__, __LINE__, #func).fail_test(e.what()); \
    } } while(0)

// function test, just runs the function without mangling its name
#define wruntest(test, ...) wobble_test_runner(wobble_test_location.nest(wobble_test_location_info, __FILE__, __LINE__, "function: " #test "(" #__VA_ARGS__ ")"), test, ##__VA_ARGS__)

#define wassert(...) wobble_test_runner(wobble_test_location.nest(wobble_test_location_info, __FILE__, __LINE__, #__VA_ARGS__), _wassert, ##__VA_ARGS__)

/**
 * Call a function returning its result, and raising TestFailed with the
 * appropriate backtrace information if it threw an exception.
 */
#define wcallchecked(func) \
    [&]() { try { \
        return func; \
    } catch (tut::failure) { \
        throw; \
    } catch (std::exception& e) { \
        wibble_test_location.fail_test(wibble_test_location_info, __FILE__, __LINE__, #func, e.what()); \
    } }()

/**
 * Base class for test fixtures.
 *
 * A fixture will have a constructor and a destructor to do setup/teardown, and
 * a reset() function to be called inbetween tests.
 *
 * Fixtures do not need to descend from Fixture: this implementation is
 * provided as a default for tests that do not need one, or as a base for
 * fixtures that do not need reset().
 */
struct Fixture
{
    // Called before each test
    void test_setup() {}

    // Called after each test
    void test_teardown() {}
};


struct TestCase;

/**
 * Result of running a test method.
 */
struct TestMethodResult
{
    /// Name of the test case
    std::string test_case;

    /// Name of the test method
    std::string test_method;

    /// If non-empty, the test failed with this error
    std::string error_message;

    /// If non-empty, the test raised an exception and this is its type ID
    std::string exception_typeid;

    /// True if the test has been skipped
    bool skipped = false;


    TestMethodResult(const std::string& test_case, const std::string& test_method)
        : test_case(test_case), test_method(test_method) {}

    void set_failed(TestFailed& e)
    {
        error_message = e.what();
        if (error_message.empty())
            error_message = "test failed with an empty error message";
    }

    void set_exception(std::exception& e)
    {
        error_message = e.what();
        if (error_message.empty())
            error_message = "test threw an exception with an empty error message";
        exception_typeid = typeid(e).name();
    }

    void set_unknown_exception()
    {
        error_message = "unknown exception caught";
    }

    bool is_success() const
    {
        return error_message.empty();
    }
};

struct TestCaseResult
{
    /// Name of the test case
    std::string test_case;
    /// Outcome of all the methods that have been run
    std::vector<TestMethodResult> methods;
    /// Set to a non-empty string if the init method of the test case failed
    std::string fail_init;
    /// Set to a non-empty string if the shutdown method of the test case
    /// failed
    std::string fail_shutdown;
    /// Set to true if this test case has been skipped
    bool skipped = false;

    TestCaseResult(const std::string& test_case) : test_case(test_case) {}

    void set_init_failed()
    {
        fail_init = "test case init method threw an unknown exception";
    }

    void set_init_failed(std::exception& e)
    {
        fail_init = "test case init method threw an exception: ";
        fail_init += e.what();
    }

    void set_shutdown_failed()
    {
        fail_shutdown = "test case shutdown method threw an unknown exception";
    }

    void set_shutdown_failed(std::exception& e)
    {
        fail_shutdown = "test case shutdown method threw an exception: ";
        fail_shutdown += e.what();
    }

    void add_test_method(TestMethodResult&& e)
    {
        methods.emplace_back(std::move(e));
    }

    bool is_success() const
    {
        if (!fail_init.empty() || !fail_shutdown.empty()) return false;
        for (const auto& m: methods)
            if (!m.is_success())
                return false;
        return true;
    }
};

struct TestController
{
    virtual ~TestController() {}

    virtual bool test_case_begin(const TestCaseResult& test_case) { return true; }
    virtual void test_case_end(const TestCaseResult& test_case) {}
    virtual bool test_method_begin(const TestMethodResult& test_method) { return true; }
    virtual void test_method_end(const TestMethodResult& test_method) {}
};

struct SimpleTestController : public TestController
{
    std::string whitelist;
    std::string blacklist;

    bool test_case_begin(const TestCaseResult& test_case) override;
    void test_case_end(const TestCaseResult& test_case) override;
    bool test_method_begin(const TestMethodResult& test_method) override;
    void test_method_end(const TestMethodResult& test_method) override;
};


struct TestRegistry
{
    std::vector<TestCase*> entries;

    void register_test_case(TestCase& test_case);

    std::vector<TestCaseResult> run_tests(TestController& controller);

    static TestRegistry& get();
};


struct TestCase
{
    struct Method
    {
        std::string name;
        std::function<void()> test_function;

        Method(const std::string& name, std::function<void()> test_function)
            : name(name), test_function(test_function) {}
    };

    std::string name;
    std::vector<Method> methods;

    TestCase(const std::string& name)
        : name(name)
    {
        TestRegistry::get().register_test_case(*this);
    }
    virtual ~TestCase() {}

    virtual void register_tests() = 0;

    virtual void init() {}
    virtual void shutdown() {}

    /**
     * init(), run all the tests that have been registered, then shutdown().
     *
     * Exceptions in init() and shutdown() are caught and reported in
     * TestCaseResult. Test are run using run_test().
     */
    virtual TestCaseResult run_tests(TestController& controller);

    /**
     * Run a test method.
     *
     * Exceptions thrown by the test method are caught and reported in
     * TestMethodResult.
     */
    virtual TestMethodResult run_test(TestController& controller, Method& method);

    void add_method(const std::string& name, std::function<void()> test_function)
    {
        methods.emplace_back(name, test_function);
    }
};

template<typename Fixture>
struct FixtureTestCase : public TestCase
{
    Fixture* fixture = 0;
    std::function<Fixture*()> make_fixture;

    template<typename... Args>
    FixtureTestCase(const std::string& name, Args... args)
        : TestCase(name)
    {
        make_fixture = [&]() { return new Fixture(&args...); };
    }

    virtual void init()
    {
        fixture = make_fixture();
    }

    virtual void shutdown()
    {
        delete fixture;
        fixture = 0;
    }

    void add_method(const std::string& name, std::function<void(Fixture&)> test_function)
    {
        methods.emplace_back(name, [&]() { test_function(*fixture); });
    }
};

#if 0
    struct Test
    {
        std::string name;
        std::function<void()> test_func;
    };

    /// Add tests to the test case
    virtual void add_tests() {}
#endif


}
}

#endif
