#include "tests.h"
#include "subprocess.h"
#include "sys.h"

using namespace std;
using namespace wobble;
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

add_method("stdout", []() {
    Popen cmd_wc;
    cmd_wc.args.push_back("wc");
    cmd_wc.args.push_back("-c");

    cmd_wc.set_stdin(Redirect::PIPE);
    cmd_wc.set_stdout(Redirect::PIPE);

    cmd_wc.fork();

    wassert(actual(cmd_wc.get_stdin()) != -1);
    wassert(actual(cmd_wc.get_stdout()) != -1);
    wassert(actual(cmd_wc.get_stderr()) == -1);

    sys::NamedFileDescriptor in(cmd_wc.get_stdin(), "stdin");
    sys::NamedFileDescriptor out(cmd_wc.get_stdout(), "stdout");

    in.write("test\n", 5);
    in.close();

    char buf[64];
    size_t len = out.read(buf, 64);

    string read(buf, len);
    cmd_wc.wait();
    wassert(actual(cmd_wc.returncode()) == 0);

    wassert(actual(read) == "5\n");
});

add_method("stdout_to_file", []() {
    Popen cmd_wc;
    cmd_wc.args.push_back("wc");
    cmd_wc.args.push_back("-c");

    sys::File fd("test.out", O_RDWR | O_CREAT);

    cmd_wc.set_stdin(Redirect::PIPE);
    cmd_wc.set_stdout(fd);

    cmd_wc.fork();

    wassert(actual(cmd_wc.get_stdin()) != -1);
    wassert(actual(cmd_wc.get_stdout()) == -1);
    wassert(actual(cmd_wc.get_stderr()) == -1);

    sys::NamedFileDescriptor in(cmd_wc.get_stdin(), "stdin");
    in.write("test\n", 5);
    in.close();

    cmd_wc.wait();
    wassert(actual(cmd_wc.returncode()) == 0);

    char buf[64];
    fd.lseek(0);
    size_t len = fd.read(buf, 64);
    string read(buf, len);

    wassert(actual(read) == "5\n");
});

add_method("stderr_to_stdout", []() {
    Popen cmd_wc;
    cmd_wc.args.push_back("ls");
    cmd_wc.args.push_back("/nonexistent");

    sys::File fd("test.out", O_RDWR | O_CREAT);

    cmd_wc.set_stdin(Redirect::DEVNULL);
    cmd_wc.set_stdout(fd);
    cmd_wc.set_stderr(Redirect::STDOUT);

    cmd_wc.fork();

    wassert(actual(cmd_wc.get_stdin()) == -1);
    wassert(actual(cmd_wc.get_stdout()) == -1);
    wassert(actual(cmd_wc.get_stderr()) == -1);

    cmd_wc.wait();
    wassert(actual(cmd_wc.returncode()) == 2);

    char buf[64];
    fd.lseek(0);
    size_t len = fd.read(buf, 64);
    string read(buf, len);

    wassert(actual(read).startswith("ls:"));
});

}

}
