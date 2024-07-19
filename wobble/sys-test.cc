#include "tests.h"
#include "sys.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstring>
#include <set>
#include <unistd.h>

using namespace std;
using namespace wobble::sys;
using namespace wobble::tests;

namespace {

class Tests : public TestCase
{
    using TestCase::TestCase;

    void register_tests() override;
} test_("sys");

void Tests::register_tests() {

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
add_method("isdir", []() {
    system("rm -rf testdir");
    wassert(actual(isdir("testdir")).isfalse());
    system("touch testdir");
    wassert(actual(isdir("testdir")).isfalse());
    system("rm testdir; mkdir testdir");
    wassert(actual(isdir("testdir")).istrue());
});

add_method("abspath", []() {
    std::string cwd = std::filesystem::current_path().string();
    wassert(actual(abspath(".")) == cwd + "/");
    wassert(actual(abspath("foo")) == cwd + "/foo");
    wassert(actual(abspath("foo/")) == cwd + "/foo/");
});
#pragma GCC diagnostic pop

add_method("timestamp", []() {
    using namespace wobble;
    system("rm -f testfile");
    wassert(actual(sys::timestamp("testfile", 0)) == 0);
    write_file("testfile", "");
    wassert(actual(sys::timestamp("testfile")) != 0);
    wassert(actual(sys::timestamp("testfile", 0)) != 0);
    unlink("testfile");
    wassert(actual(sys::timestamp("testfile", 0)) == 0);
});

add_method("write_file_atomically", []() {
    string test("ciao");
    write_file_atomically("testfile", test);
    string test1 = read_file("testfile");
    wassert(actual(test1) == test);

    write_file("testfile", "");
    wassert(actual(read_file("testfile")) == "");
});

add_method("directory_iterate", []() {
    Path dir("/", O_DIRECTORY);

    set<string> files;
    for (auto& i: dir)
        files.insert(i.d_name);

    wassert(actual(files.size()) > 0u);
    wassert(actual(files.find(".") != files.end()).istrue());
    wassert(actual(files.find("..") != files.end()).istrue());
    wassert(actual(files.find("etc") != files.end()).istrue());
    wassert(actual(files.find("bin") != files.end()).istrue());
    wassert(actual(files.find("tmp") != files.end()).istrue());

    // Check that the directory can be iterated twice in a row
    files.clear();
    for (auto& i: dir)
        files.insert(i.d_name);

    wassert(actual(files.size()) > 0u);
    wassert(actual(files.find(".") != files.end()).istrue());
    wassert(actual(files.find("..") != files.end()).istrue());
    wassert(actual(files.find("etc") != files.end()).istrue());
    wassert(actual(files.find("bin") != files.end()).istrue());
    wassert(actual(files.find("tmp") != files.end()).istrue());

    struct stat st;
    dir.fstatat("etc", st);
    wassert(actual(S_ISDIR(st.st_mode)).istrue());

    wassert(actual(dir.fstatat_ifexists("wobble_unit_test_file_expected_not_to_be_there", st)).isfalse());

    wassert(actual(dir.faccessat("etc", X_OK)).istrue());
    wassert(actual(dir.faccessat("wobble_unit_test_file_expected_not_to_be_there", F_OK)).isfalse());
});

add_method("openat_ifexists", []() {
    Path dir("/etc", O_DIRECTORY);

    int fd = dir.openat_ifexists("passwd", O_RDONLY);
    wassert(actual(fd) != -1);
    ::close(fd);

    fd = dir.openat_ifexists("does-not-exist-really", O_RDONLY);
    wassert(actual(fd) == -1);
});


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
add_method("makedirs", []() {
    wassert(actual(makedirs("makedirs/foo/bar/baz")).istrue());
    wassert(actual(std::filesystem::is_directory("makedirs/foo/bar/baz")).istrue());
    wassert(actual(makedirs("makedirs/foo/bar/baz")).isfalse());
    wassert(actual(std::filesystem::is_directory("makedirs/foo/bar/baz")).istrue());
});
#pragma GCC diagnostic pop

add_method("rmtree", []() {
    namespace fs = std::filesystem;
    auto root = fs::path("foo/bar");
    fs::create_directories(root / "baz");
    fs::create_directories(root / "gnat");
    write_file("foo/bar/baz.txt", "baz");
    write_file("foo/bar/baz/gnat.txt", "gnat");
    write_file("foo/bar.txt", "bar");
    rmtree("foo");
    wassert(actual_file("foo").not_exists());

    rmtree_ifexists("foo");
});

add_method("which", []() {
    wassert(actual(which("ls")).endswith("bin/ls"));
});

add_method("unlink_ifexists", []() {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    std::filesystem::path fname = "test_unlink_ifexists";

    unlink_ifexists(fname);
    wassert(actual(unlink_ifexists(fname)).isfalse());
    write_file(fname, "test");
    wassert(actual(fname).exists());
    wassert(actual(unlink_ifexists(fname)).istrue());
    wassert(actual(fname).not_exists());
#pragma GCC diagnostic pop
});

add_method("rename_ifexists", []() {
    std::filesystem::path fname = "test_rename_ifexists";
    std::filesystem::path fname1 = "test_rename_ifexists1";

    std::filesystem::remove(fname);
    std::filesystem::remove(fname1);
    wassert(actual(rename_ifexists(fname, fname1)).isfalse());

    write_file(fname, "test");
    wassert(actual(fname).exists());
    wassert(actual(fname1).not_exists());

    wassert(actual(rename_ifexists(fname, fname1)).istrue());
    wassert(actual(fname).not_exists());
    wassert(actual(fname1).exists());

    unlink(fname1);
});

add_method("rename", []() {
    std::filesystem::path fname = "test_rename";
    std::filesystem::path fname1 = "test_rename1";

    write_file(fname, "test");
    rename(fname, fname1);
    wassert(actual(fname).not_exists());
    wassert(actual(fname1).exists());
    unlink(fname1);
});

add_method("file", []() {
    File f("test", O_RDWR | O_CREAT, 0666);
    wassert(actual(f.write("foo", 3)) == 3u);
    wassert(actual(f.lseek(0)) == 0);
    char buf[4];
    wassert(actual(f.read(buf, 3)) == 3u);
    buf[3] = 0;
    wassert(actual(buf) == "foo");

    wassert(actual(f.pwrite("ar", 2, 1)) == 2u);
    wassert(actual(f.pread(buf, 3, 0)) == 3u);
    wassert(actual(buf) == "far");

    wassert(actual(f.pwrite(string("oz"), 1)) == 2u);
    wassert(actual(f.pread(buf, 3, 0)) == 3u);
    wassert(actual(buf) == "foz");

    f.close();

    wassert(actual(f.open_ifexists(O_RDONLY)));
    f.close();

    File f1("test-does-not-exists");
    wassert(actual(f1.open_ifexists(O_RDONLY)).isfalse());
});

add_method("ofd_lock", []() {
    File f1("test", O_RDWR | O_CREAT, 0666);
    File f2("test", O_RDWR);

    ::flock lk1;
    memset(&lk1, 0, sizeof(lk1));
    ::flock lk2;
    memset(&lk2, 0, sizeof(lk2));

    lk1.l_type = F_RDLCK;
    lk1.l_whence = SEEK_SET;
    lk1.l_start = 0;
    lk1.l_len = 10;
    wassert(actual(f1.ofd_setlkw(lk1, true)).istrue());

    lk2.l_type = F_RDLCK;
    lk2.l_whence = SEEK_SET;
    lk2.l_start = 0;
    lk2.l_len = 10;
    wassert(actual(f2.ofd_setlkw(lk2, true)).istrue());

    lk2.l_type = F_WRLCK;
    wassert(actual(f2.ofd_setlk(lk2)).isfalse());

    ::flock lk3;
    memset(&lk3, 0, sizeof(lk3));
    lk3.l_type = F_WRLCK;
    lk3.l_whence = SEEK_SET;
    lk3.l_start = 5;
    lk3.l_len = 100;
    wassert(actual(f2.ofd_getlk(lk3)).isfalse());
    wassert(actual(lk3.l_type) == F_RDLCK);
    wassert(actual(lk3.l_whence) == SEEK_SET);
    wassert(actual(lk3.l_start) == 0);
    wassert(actual(lk3.l_len) == 10);

    lk1.l_type = F_UNLCK;
    wassert(actual(f1.ofd_setlkw(lk1, true)).istrue());

    wassert(actual(f2.ofd_setlk(lk2)).istrue());
});

add_method("preserve_file_times", []() {
    File test("test", O_RDWR | O_CREAT, 0666);
    ::timespec ts[2] = { { 1500000000, 0 }, { 1500000000, 0 } };
    test.futimens(ts);

    {
        PreserveFileTimes pt(test);
        test.write("test", 4);
    }

    struct stat st;
    test.fstat(st);

    wassert(actual(st.st_atim.tv_sec) == 1500000000);
    wassert(actual(st.st_atim.tv_nsec) == 0);
    wassert(actual(st.st_mtim.tv_sec) == 1500000000);
    wassert(actual(st.st_mtim.tv_nsec) == 0);
});

add_method("touch", []() {
    write_file("test", "foo");
    touch("test", 123456);

    struct stat st;
    stat("test", st);

    wassert(actual(st.st_atim.tv_sec) == 123456);
    wassert(actual(st.st_atim.tv_nsec) == 0);
    wassert(actual(st.st_mtim.tv_sec) == 123456);
    wassert(actual(st.st_mtim.tv_nsec) == 0);
});

add_method("timespec_elapsed", []() {
    ::timespec begin, until;

    begin.tv_sec = 100; begin.tv_nsec = 50;
    until.tv_sec = 100; until.tv_nsec = 60;
    wassert(actual(timesec_elapsed(begin, until)) == 10u);

    begin.tv_sec = 100; begin.tv_nsec = 50;
    until.tv_sec = 101; until.tv_nsec = 40;
    wassert(actual(timesec_elapsed(begin, until)) == 1000000000u - 10u);

    begin.tv_sec = 100; begin.tv_nsec = 50;
    until.tv_sec = 101; until.tv_nsec = 60;
    wassert(actual(timesec_elapsed(begin, until)) == 1000000000u + 10u);

    begin.tv_sec = 100; begin.tv_nsec = 0;
    until.tv_sec = 101; until.tv_nsec = 0;
    wassert(actual(timesec_elapsed(begin, until)) == 1000000000u);

    begin.tv_sec = 101; begin.tv_nsec = 0;
    until.tv_sec = 100; until.tv_nsec = 0;
    wassert(actual(timesec_elapsed(begin, until)) == 0u);

    begin.tv_sec = 100; begin.tv_nsec = 5;
    until.tv_sec = 100; until.tv_nsec = 4;
    wassert(actual(timesec_elapsed(begin, until)) == 0u);
});

add_method("rlimit", []() {
    File fd("testfile", O_WRONLY | O_CREAT | O_TRUNC);

    ::rlimit rlim_pre;
    getrlimit(RLIMIT_NOFILE, rlim_pre);

    {
        OverrideRlimit ov(RLIMIT_NOFILE, 0);
        ::rlimit rlim_cur;
        getrlimit(RLIMIT_NOFILE, rlim_cur);
        wassert(actual(rlim_cur.rlim_max) == rlim_pre.rlim_max);
        wassert(actual(rlim_cur.rlim_cur) == 0u);

        int dupfd = ::dup(fd);
        if (dupfd != -1) ::close(dupfd);
        wassert(actual(dupfd == -1));
        wassert(actual(errno) == EMFILE);
    }

    ::rlimit rlim_post;
    getrlimit(RLIMIT_NOFILE, rlim_post);
    wassert(actual(rlim_post.rlim_max) == rlim_pre.rlim_max);
    wassert(actual(rlim_post.rlim_cur) == rlim_pre.rlim_cur);

    int dupfd = ::dup(fd);
    if (dupfd != -1) ::close(dupfd);
    wassert(actual(dupfd >= 0));
});

add_method("tempfile", []() {
    std::filesystem::path path;
    {
        Tempfile tf;
        wassert(actual(tf.path()).exists());
        path = tf.path();
    }
    wassert(actual(path).not_exists());

    {
        Tempfile tf;
        wassert(actual(tf.path()).exists());
        tf.unlink_on_exit(false);
        path = tf.path();
    }
    wassert(actual_file(path).exists());

    unlink(path);

    {
        Tempfile tf;
        wassert(actual(tf.path()).exists());
        tf.unlink();
        wassert(actual(tf.path()).not_exists());
    }

    {
        Tempfile tf("wibble-test-");
        wassert(actual(tf.path().filename().string()).startswith("wibble-test-"));
    }
});

add_method("mkdtemp", []() {
    std::filesystem::path path = Path::mkdtemp("./test");
    wassert_true(std::filesystem::is_directory(path));
    wassert(actual(path.string()).startswith("./test"));
    rmdir(path);
});

add_method("tempdir", []() {
    std::filesystem::path path;
    {
        Tempdir dir;
        wassert_true(std::filesystem::is_directory(dir.path()));
        FileDescriptor fd(dir.openat("test", O_WRONLY | O_CREAT));
        fd.close();
        wassert_true(std::filesystem::is_regular_file(dir.path() / "test"));
        path = dir.path();
    }
    wassert_false(exists(path));

    {
        Tempdir dir;
        wassert_true(std::filesystem::is_directory(dir.path()));
        dir.rmtree_on_exit(false);
        path = dir.path();
    }
    wassert_true(exists(path));
    rmtree(path);
});

add_method("mkdirat", []() {
    Tempdir dir;
    dir.mkdirat("test");
    wassert_true(std::filesystem::is_directory(dir.path() / "test"));
});

add_method("symlinkat", []() {
    Tempdir dir;
    dir.symlinkat("/etc", "etc");

    struct stat st;
    wassert_true(dir.lstatat_ifexists("etc", st));

    wassert(actual(dir.readlinkat("etc")) == "/etc");
});

}

#if 0

struct TestFs {

    Test directoryIsdir()
    {
        {
            Directory dir("/");
            for (Directory::const_iterator i = dir.begin(); i != dir.end(); ++i)
                if (*i == "etc")
                {
                    assert(i.isdir());
                    assert(!i.isreg());
                }
        }
        {
            Directory dir("/etc");
            for (Directory::const_iterator i = dir.begin(); i != dir.end(); ++i)
                if (*i == "passwd")
                {
                    assert(i.isreg());
                    assert(!i.isdir());
                }
        }
        {
            Directory dir("/dev");
            for (Directory::const_iterator i = dir.begin(); i != dir.end(); ++i)
            {
                if (*i == "null")
                {
                    assert(i.ischr());
                    assert(!i.isblk());
                }
                else if (*i == "sda")
                {
                    assert(i.isblk());
                    assert(!i.ischr());
                }
            }
        }
    }

    // Ensure that nonexisting directories and files are reported as not valid
    Test invalidDirectories() {
#ifdef POSIX
        Directory dir1("/antaniblindalasupercazzola123456");
        assert(!dir1.exists());
        try {
            Directory::const_iterator i = dir1.begin();
            assert(false);
        } catch (wibble::exception::System& e) {
        }

        Directory dir2("/etc/passwd");
        assert(!dir2.exists());
        try {
            Directory::const_iterator i = dir2.begin();
            assert(false);
        } catch (wibble::exception::System& e) {
        }
#endif
    }

    Test _mkPath() {
#ifdef POSIX
        // Mkpath should succeed on existing directory
        mkpath(".");

        // Mkpath should succeed on existing directory
        mkpath("./.");

        // Mkpath should succeed on existing directory
        mkpath("/");
#endif
    }

    Test _mkPath2() {
#ifdef POSIX
        // Try creating a path with mkpath
        system("rm -rf test-mkpath");
        mkpath("test-mkpath/test-mkpath");
        assert(wibble::sys::fs::access("test-mkpath", F_OK));
        assert(wibble::sys::fs::access("test-mkpath/test-mkpath", F_OK));
        system("rm -rf test-mkpath");
#endif
    }

    Test _mkFilePath() {
#ifdef POSIX
        // Try creating a path with mkFilePath
        system("rm -rf test-mkpath");
        mkFilePath("test-mkpath/test-mkpath/file");
        assert(wibble::sys::fs::access("test-mkpath", F_OK));
        assert(wibble::sys::fs::access("test-mkpath/test-mkpath", F_OK));
        assert(!wibble::sys::fs::access("test-mkpath/test-mkpath/file", F_OK));
        system("rm -rf test-mkpath");
#endif
    }

    Test _mkdirIfMissing() {
        // Creating works and is idempotent
        {
            system("rm -rf test-mkpath");
            assert(!wibble::sys::fs::access("test-mkpath", F_OK));
            wibble::sys::fs::mkdirIfMissing("test-mkpath");
            assert(wibble::sys::fs::access("test-mkpath", F_OK));
            wibble::sys::fs::mkdirIfMissing("test-mkpath");
        }

        // Creating fails if it exists and it is a file
        {
            system("rm -rf test-mkpath; touch test-mkpath");
            try {
                wibble::sys::fs::mkdirIfMissing("test-mkpath");
                assert(false);
            } catch (wibble::exception::Consistency& e) {
                assert(string(e.what()).find("exists but it is not a directory") != string::npos);
            }
        }

        // Deal with dangling symlinks
        {
            system("rm -rf test-mkpath; ln -s ./tmp/tmp/tmp/DOESNOTEXISTS test-mkpath");
            try {
                wibble::sys::fs::mkdirIfMissing("test-mkpath");
                assert(false);
            } catch (wibble::exception::Consistency& e) {
                assert(string(e.what()).find("looks like a dangling symlink") != string::npos);
            }
        }
    }

    Test _deleteIfExists() {
#ifdef POSIX
        system("rm -f does-not-exist");
        assert(!deleteIfExists("does-not-exist"));
        system("touch does-exist");
        assert(deleteIfExists("does-exist"));
#endif
    }
};

#endif
}
