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

add_method("env_add", []() {
    Popen cmd_env({"sh", "-c", "echo $TEST_VALUE"});
    cmd_env.copy_env_from_parent();
    cmd_env.setenv("TEST_VALUE", "testvalue");
    cmd_env.set_stdout(Redirect::PIPE);
    cmd_env.fork();

    sys::NamedFileDescriptor out(cmd_env.get_stdout(), "stdout");
    char buf[64];
    size_t len = out.read(buf, 64);

    string read(buf, len);
    cmd_env.wait();
    wassert(actual(cmd_env.returncode()) == 0);

    wassert(actual(read) == "testvalue\n");
});

add_method("env_change", []() {
    Popen cmd_env({"sh", "-c", "echo $USER"});
    cmd_env.copy_env_from_parent();
    cmd_env.setenv("USER", "testvalue");
    cmd_env.set_stdout(Redirect::PIPE);
    cmd_env.fork();

    sys::NamedFileDescriptor out(cmd_env.get_stdout(), "stdout");
    char buf[64];
    size_t len = out.read(buf, 64);

    string read(buf, len);
    cmd_env.wait();
    wassert(actual(cmd_env.returncode()) == 0);

    wassert(actual(read) == "testvalue\n");
});

add_method("wait", [] {
    Popen cmd({"sleep", "0.05"});
    cmd.fork();
    wassert_true(cmd.started());
    wassert_false(cmd.terminated());
    wassert(actual(cmd.wait()) == 0);
    wassert(actual(cmd.returncode()) == 0);
    wassert_true(cmd.started());
    wassert_true(cmd.terminated());
});

add_method("wait1", [] {
    Popen cmd({"sleep", "0.05"});
    cmd.fork();
    wassert_true(cmd.started());
    wassert_false(cmd.terminated());
    wassert_false(cmd.wait(10));

    wassert_true(cmd.started());
    wassert_false(cmd.terminated());
    wassert(actual(cmd.returncode()) == 0);

    ::timespec pre;
    wassert(actual(clock_gettime(CLOCK_MONOTONIC, &pre)) == 0);

    wassert_true(cmd.wait(1000));

    ::timespec post;
    wassert(actual(clock_gettime(CLOCK_MONOTONIC, &post)) == 0);

    double elapsed = static_cast<double>(post.tv_sec - pre.tv_sec) + static_cast<double>(post.tv_nsec - pre.tv_nsec) / 1000000000.0;
    wassert(actual(elapsed) <= 0.1);

    wassert_true(cmd.started());
    wassert_true(cmd.terminated());
    wassert(actual(cmd.returncode()) == 0);
});

}

}
