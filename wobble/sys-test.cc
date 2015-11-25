#include "tests.h"
#include "sys.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
//#include <cstdlib>
#include <set>
//#include <unistd.h>

using namespace std;
using namespace wobble::sys;
using namespace wobble::tests;

namespace {

class Tests : public TestCase
{
    using TestCase::TestCase;

    void register_tests() override
    {
        add_method("isdir", []() {
            system("rm -rf testdir");
            wassert(actual(isdir("testdir")).isfalse());
            system("touch testdir");
            wassert(actual(isdir("testdir")).isfalse());
            system("rm testdir; mkdir testdir");
            wassert(actual(isdir("testdir")).istrue());
        });

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
        });

        add_method("directory_iterate", []() {
            Path dir("/", O_DIRECTORY);

            set<string> files;
            for (auto& i: dir)
                files.insert(i.d_name);

            wassert(actual(files.size()) > 0);
            wassert(actual(files.find(".") != files.end()).istrue());
            wassert(actual(files.find("..") != files.end()).istrue());
            wassert(actual(files.find("etc") != files.end()).istrue());
            wassert(actual(files.find("bin") != files.end()).istrue());
            wassert(actual(files.find("tmp") != files.end()).istrue());

            // Check that the directory can be iterated twice in a row
            files.clear();
            for (auto& i: dir)
                files.insert(i.d_name);

            wassert(actual(files.size()) > 0);
            wassert(actual(files.find(".") != files.end()).istrue());
            wassert(actual(files.find("..") != files.end()).istrue());
            wassert(actual(files.find("etc") != files.end()).istrue());
            wassert(actual(files.find("bin") != files.end()).istrue());
            wassert(actual(files.find("tmp") != files.end()).istrue());
        });

        add_method("rmtree", []() {
            makedirs("foo/bar/baz");
            makedirs("foo/bar/gnat");
            write_file("foo/bar/baz.txt", "baz");
            write_file("foo/bar/baz/gnat.txt", "gnat");
            write_file("foo/bar.txt", "bar");
            rmtree("foo");
            wassert(actual(exists("foo")).isfalse());
        });

        add_method("which", []() {
            wassert(actual(which("ls")) == "/bin/ls");
        });

        add_method("unlink_ifexists", []() {
            const char* fname = "test_unlink_ifexists";

            unlink_ifexists(fname);
            wassert(actual(unlink_ifexists(fname)).isfalse());
            write_file(fname, "test");
            wassert(actual(exists(fname)).istrue());
            wassert(actual(unlink_ifexists(fname)).istrue());
            wassert(actual(exists(fname)).isfalse());
        });

        add_method("rename_ifexists", []() {
            const char* fname = "test_rename_ifexists";
            const char* fname1 = "test_rename_ifexists1";

            unlink_ifexists(fname);
            unlink_ifexists(fname1);
            wassert(actual(rename_ifexists(fname, fname1)).isfalse());

            write_file(fname, "test");
            wassert(actual(exists(fname)).istrue());
            wassert(actual(exists(fname1)).isfalse());

            wassert(actual(rename_ifexists(fname, fname1)).istrue());
            wassert(actual(exists(fname)).isfalse());
            wassert(actual(exists(fname1)).istrue());

            unlink(fname1);
        });

        add_method("file", []() {
            File f("test", O_RDWR | O_CREAT, 0666);
            wassert(actual(f.write("foo", 3)) == 3);
            wassert(actual(f.lseek(0)) == 0);
            char buf[4];
            wassert(actual(f.read(buf, 3)) == 3);
            buf[3] == 0;
            wassert(actual(buf) == "foo");

            wassert(actual(f.pwrite("ar", 2, 1)) == 2);
            wassert(actual(f.pread(buf, 3, 0)) == 3);
            wassert(actual(buf) == "far");

            f.close();
        });
    }
} test("sys");

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
