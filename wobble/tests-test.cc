#include "tests.h"
#include "sys.h"

using namespace std;
using namespace wobble;
using namespace wobble::tests;

namespace {

class Tests : public TestCase
{
    using TestCase::TestCase;

    void register_tests() override
    {
        add_method("bools", []() {
            wassert(actual(true).istrue());
            wassert(actual(false).isfalse());
        });

        add_method("int", []() {
            wassert(actual(1).istrue());
            wassert(actual(0).isfalse());
            wassert(actual(1) == 1);
            wassert(actual(1) != 2);
            wassert(actual(1) < 2);
            wassert(actual(1) <= 1);
            wassert(actual(1) <= 2);
            wassert(actual(1) > 0);
            wassert(actual(1) >= 1);
            wassert(actual(1) >= 0);
        });

        add_method("double", []() {
            wassert(actual(1.0) < 2);
            wassert(actual(1.0) <= 1);
            wassert(actual(1.0) <= 2);
            wassert(actual(1.0) > 0);
            wassert(actual(1.0) >= 1);
            wassert(actual(1.0) >= 0);
            wassert(actual(1.0001).almost_equal(1.0002, 3));
            wassert(actual(1.0001).not_almost_equal(1.0002, 4));
            wassert(actual(1.0001).not_almost_equal(2.0, 2));
        });

        add_method("pointers", []() {
            wassert(actual(nullptr).isfalse());
            wassert(actual(static_cast<const void*>("foo")).istrue());
        });

        add_method("cstrings", []() {
            wassert(actual(nullptr).isfalse());
            wassert(actual("").istrue());
            wassert(actual("foo").istrue());
            wassert(actual("foo") == "foo");
            wassert(actual("foo") != "bar");
            wassert(actual("bar") < "foo");
            wassert(actual("bar") <= "bar");
            wassert(actual("bar") <= "foo");
            wassert(actual("foo") > "bar");
            wassert(actual("foo") >= "foo");
            wassert(actual("foo") >= "bar");
            wassert(actual("foo").startswith("fo"));
            wassert(actual("foo").endswith("oo"));
            wassert(actual("foo").contains("o"));
            wassert(actual("foo").matches("^fo"));
            wassert(actual("foo").not_matches("^oo"));
        });

        add_method("stdstrings", []() {
            wassert(actual(string("foo")) == string("foo"));
            wassert(actual(string("foo")) != string("bar"));
            wassert(actual(string("bar")) < string("foo"));
            wassert(actual(string("bar")) <= string("bar"));
            wassert(actual(string("bar")) <= string("foo"));
            wassert(actual(string("foo")) > string("bar"));
            wassert(actual(string("foo")) >= string("foo"));
            wassert(actual(string("foo")) >= string("bar"));
            wassert(actual(string("foo")).startswith("fo"));
            wassert(actual(string("foo")).endswith("oo"));
            wassert(actual(string("foo")).contains("o"));
            wassert(actual(string("foo")).matches("^fo"));
            wassert(actual(string("foo")).not_matches("^oo"));
        });

        add_method("strings", []() {
            wassert(actual(string("foo")) == "foo");
            wassert(actual(string("foo")) != "bar");
            wassert(actual(string("bar")) < "foo");
            wassert(actual(string("bar")) <= "bar");
            wassert(actual(string("bar")) <= "foo");
            wassert(actual(string("foo")) > "bar");
            wassert(actual(string("foo")) >= "foo");
            wassert(actual(string("foo")) >= "bar");

            wassert(actual("foo") == string("foo"));
            wassert(actual("foo") != string("bar"));
            wassert(actual("bar") < string("foo"));
            wassert(actual("bar") <= string("bar"));
            wassert(actual("bar") <= string("foo"));
            wassert(actual("foo") > string("bar"));
            wassert(actual("foo") >= string("foo"));
            wassert(actual("foo") >= string("bar"));

            std::vector<uint8_t> buf;
            buf.push_back('a'); buf.push_back('b'); buf.push_back('c');
            wassert(actual(buf) == "abc");
            wassert(actual(string("abc")) == buf);
        });

        add_method("paths", []() {
            using path = std::filesystem::path;
            wassert(actual(path("foo")) == path("foo"));
            wassert(actual(path("foo")) != path("bar"));
            wassert(actual(path("bar")) < path("foo"));
            wassert(actual(path("bar")) <= path("bar"));
            wassert(actual(path("bar")) <= path("foo"));
            wassert(actual(path("foo")) > path("bar"));
            wassert(actual(path("foo")) >= path("foo"));
            wassert(actual(path("foo")) >= path("bar"));
            wassert(actual(path("foo")).is("foo"));
            wassert(actual(path("foo/.")).is("foo/"));
            wassert(actual(path("foo/../bar")).is("baz/foo/../../bar"));
            wassert(actual(path("foo")).startswith("foo"));
            wassert(actual(path("foo/bar")).startswith("foo"));
            wassert(actual(path("foo/bar/baz")).startswith("foo/bar"));
            wassert(actual(path("foo")).endswith("foo"));
            wassert(actual(path("foo/bar")).endswith("bar"));
            wassert(actual(path("foo/bar/baz")).endswith("bar/baz"));
            wassert(actual(path("foo")).contains("foo"));
            wassert(actual(path("foo/bar/baz")).contains("foo/bar"));
            wassert(actual(path("foo/bar/baz")).contains("bar"));
            wassert(actual(path("foo/bar/baz")).contains("bar/baz"));
            wassert(actual(path("foo/bar/baz")).contains("baz"));
        });

        add_method("function", []() {
            wassert(actual_function([]() { throw std::runtime_error("foobar"); }).throws("ooba"));
        });

        add_method("skip", []() {
            throw TestSkipped("This method is intentionally skipped");
        });

        add_method("file", []() {
            sys::write_file("testfile", "");
            wassert(actual_file("testfile").empty());
            wassert(actual_file("testfile").contents_equal(""));
            wassert(actual_file("testfile").contents_equal(std::vector<uint8_t>()));

            sys::write_file("testfile", "foo");
            wassert(actual_file("testfile").exists());
            wassert(actual_file("testfile-foobarbaz").not_exists());
            wassert(actual_file("testfile").contents_startwith(""));
            wassert(actual_file("testfile").contents_startwith("f"));
            wassert(actual_file("testfile").contents_startwith("fo"));
            wassert(actual_file("testfile").contents_startwith("foo"));
            wassert(actual_file("testfile").not_empty());
            wassert(actual_file("testfile").contents_equal("foo"));
            std::vector<uint8_t> data;
            data.push_back('f');
            data.push_back('o');
            data.push_back('o');
            wassert(actual_file("testfile").contents_equal(data));
            wassert(actual_file("testfile").contents_match("^f.+o$"));

            sys::write_file("testfile", "foo\nbar\n");
            wassert(actual_file("testfile").contents_equal({"foo", "bar"}));
            wassert(actual_file("testfile").contents_match({"^foo", "bar$"}));
            wassert(actual_file("testfile").contents_match({"^foo", "(maybe)?", "bar$"}));
            wassert(actual_file("testfile").contents_match({"^foo", "bar$", "(maybe)?"}));
            wassert(actual_file("testfile").contents_match({"(maybe)?", "^foo", "bar$"}));
        });

        add_method("empty_skipped");

        add_method("throws", []{
            auto e = wassert_throws(std::runtime_error, throw std::runtime_error("expected exception"));
            wassert(actual(e.what()) == "expected exception");
        });
    }
} tests("tests");


struct ValueFixture : public wobble::tests::Fixture
{
    int val = 0;
};

struct TestFixture : public FixtureTestCase<ValueFixture>
{
    using FixtureTestCase::FixtureTestCase;

    void register_tests() override
    {
        add_method("fixture", [](Fixture& f) {
            wassert(actual(f.val) == 0);
        });
    }
} tests1("tests_fixture");

struct SkipFixture : public wobble::tests::Fixture
{
    SkipFixture() { throw TestSkipped("This test case is intentionally skipped"); }
};

struct TestSkipFixture : public FixtureTestCase<SkipFixture>
{
    using FixtureTestCase::FixtureTestCase;

    void register_tests() override
    {
        add_method("fails", [](Fixture&) {
            wfail_test("This should never run");
        });
    }
} tests2("tests_skipfixture");

}

