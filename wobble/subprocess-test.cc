#include "tests.h"
#include "subprocess.h"

using namespace std;
using namespace wobble::subprocess;
using namespace wobble::tests;

namespace {

class Tests : public TestCase
{
    using TestCase::TestCase;

    void register_tests() override;
} test("subprocess");

void Tests::register_tests() {

add_method("true", []() {
    Popen cmd_true;
    wassert_false(cmd_true.started());
    wassert_false(cmd_true.terminated());

    cmd_true.args.push_back("true");
    wassert_false(cmd_true.started());
    wassert_false(cmd_true.terminated());

    cmd_true.fork();
    wassert_true(cmd_true.started());
    wassert_false(cmd_true.terminated());

    cmd_true.wait();
    wassert_true(cmd_true.started());
    wassert_true(cmd_true.terminated());

    wassert(actual(cmd_true.returncode()) == 0);
});

add_method("false", []() {
    Popen cmd_false;
    wassert_false(cmd_false.started());
    wassert_false(cmd_false.terminated());

    cmd_false.args.push_back("false");
    wassert_false(cmd_false.started());
    wassert_false(cmd_false.terminated());

    cmd_false.fork();
    wassert_true(cmd_false.started());
    wassert_false(cmd_false.terminated());

    cmd_false.wait();
    wassert_true(cmd_false.started());
    wassert_true(cmd_false.terminated());

    wassert(actual(cmd_false.returncode()) == 1);
});

}

}
