#include "tests.h"
#include "string.h"

using namespace std;
using namespace wobble;
using namespace wobble::tests;

namespace {

class Tests : public TestCase
{
    using TestCase::TestCase;

    void register_tests() override
    {
        add_method("startswith", []() {
            wassert(actual(str::startswith("ciao", "ci")).istrue());
            wassert(actual(str::startswith("ciao", "")).istrue());
            wassert(actual(str::startswith("ciao", "ciao")).istrue());
            wassert(actual(str::startswith("ciao", "ciaoa")).isfalse());
            wassert(actual(str::startswith("ciao", "i")).isfalse());
        });

        add_method("endswith", []() {
            wassert(actual(str::endswith("ciao", "ao")).istrue());
            wassert(actual(str::endswith("ciao", "")).istrue());
            wassert(actual(str::endswith("ciao", "ciao")).istrue());
            wassert(actual(str::endswith("ciao", "aciao")).isfalse());
            wassert(actual(str::endswith("ciao", "a")).isfalse());
        });

        add_method("join", []() {
            vector<int> nums { 1, 2, 3 };
            wassert(actual(str::join(":", nums)) == "1:2:3");

            vector<std::string> strs { "", "foo", "", "", "bar" };
            wassert(actual(str::join(",", strs)) == ",foo,,,bar");
            wassert(actual(str::join(",", strs.begin(), strs.end())) == ",foo,,,bar");
        });

        add_method("strip", []() {
            wassert(actual(str::strip("   ")) == "");
            wassert(actual(str::strip(" c  ")) == "c");
            wassert(actual(str::strip("ciao")) == "ciao");
            wassert(actual(str::strip(" ciao")) == "ciao");
            wassert(actual(str::strip("    ciao")) == "ciao");
            wassert(actual(str::strip("ciao ")) == "ciao");
            wassert(actual(str::strip("ciao    ")) == "ciao");
            wassert(actual(str::strip(" ciao ")) == "ciao");
            wassert(actual(str::strip("      ciao    ")) == "ciao");
            //wassert(actual(str::strip(string("ciao"), ::isalpha)) == "");
            //wassert(actual(str::strip(string("ci ao"), ::isalpha)) == " ");
            //wassert(actual(str::strip(" ", ::isalpha)) == " ");

            wassert(actual(str::lstrip("   ")) == "");
            wassert(actual(str::lstrip(" c  ")) == "c  ");
            wassert(actual(str::lstrip("ciao")) == "ciao");
            wassert(actual(str::lstrip(" ciao")) == "ciao");
            wassert(actual(str::lstrip("    ciao")) == "ciao");
            wassert(actual(str::lstrip("ciao ")) == "ciao ");
            wassert(actual(str::lstrip("ciao    ")) == "ciao    ");
            wassert(actual(str::lstrip(" ciao ")) == "ciao ");
            wassert(actual(str::lstrip("      ciao    ")) == "ciao    ");
            //wassert(actual(str::lstrip(string("ciao"), ::isalpha)) == "");
            //wassert(actual(str::lstrip(string("ci ao"), ::isalpha)) == " ao");
            //wassert(actual(str::lstrip(" ", ::isalpha)) == " ");

            wassert(actual(str::rstrip("   ")) == "");
            wassert(actual(str::rstrip(" c  ")) == " c");
            wassert(actual(str::rstrip("ciao")) == "ciao");
            wassert(actual(str::rstrip(" ciao")) == " ciao");
            wassert(actual(str::rstrip("    ciao")) == "    ciao");
            wassert(actual(str::rstrip("ciao ")) == "ciao");
            wassert(actual(str::rstrip("ciao    ")) == "ciao");
            wassert(actual(str::rstrip(" ciao ")) == " ciao");
            wassert(actual(str::rstrip("      ciao    ")) == "      ciao");
            //wassert(actual(str::rstrip(string("ciao"), ::isalpha)) == "");
            //wassert(actual(str::rstrip(string("ci ao"), ::isalpha)) == "ci ");
            //wassert(actual(str::rstrip(" ", ::isalpha)) == " ");
        });

        add_method("upper", []() {
            wassert(actual(str::lower("ciao")) == "ciao");
            wassert(actual(str::lower("CIAO")) == "ciao");
            wassert(actual(str::lower("Ciao")) == "ciao");
            wassert(actual(str::lower("cIAO")) == "ciao");
        });

        add_method("lower", []() {
            wassert(actual(str::upper("ciao")) == "CIAO");
            wassert(actual(str::upper("CIAO")) == "CIAO");
            wassert(actual(str::upper("Ciao")) == "CIAO");
            wassert(actual(str::upper("cIAO")) == "CIAO");
        });

        add_method("basename", []() {
            wassert(actual(str::basename("ciao")) == "ciao");
            wassert(actual(str::basename("a/ciao")) == "ciao");
            wassert(actual(str::basename("a/b/c/c/d/e/ciao")) == "ciao");
            wassert(actual(str::basename("/ciao")) == "ciao");
        });

        add_method("dirname", []() {
            wassert(actual(str::dirname("ciao")) == ".");
            wassert(actual(str::dirname("a/ciao")) == "a");
            wassert(actual(str::dirname("a/b/c/c/d/e/ciao")) == "a/b/c/c/d/e");
            wassert(actual(str::dirname("/a/ciao")) == "/a");
            wassert(actual(str::dirname("/ciao")) == "/");
            wassert(actual(str::dirname("/ciao///")) == "/");
            wassert(actual(str::dirname("/a//b/")) == "/a");
            wassert(actual(str::dirname("")) == ".");
            wassert(actual(str::dirname("ciao///")) == ".");
        });

        add_method("joinpath", []() {
            wassert(actual(str::joinpath(string("a"), "b")) == "a/b");
            char b[] = "b";
            wassert(actual(str::joinpath(string("a"), b)) == "a/b");
            wassert(actual(str::joinpath("a", "b")) == "a/b");
            wassert(actual(str::joinpath("a/", "b")) == "a/b");
            wassert(actual(str::joinpath("a", "/b")) == "a/b");
            wassert(actual(str::joinpath("a/", "/b")) == "a/b");
            wassert(actual(str::joinpath("a/", "/b", string("c"), "/d/")) == "a/b/c/d/");
        });

        add_method("normpath", []() {
            wassert(actual(str::normpath("")) == ".");
            wassert(actual(str::normpath("/")) == "/");
            wassert(actual(str::normpath("foo")) == "foo");
            wassert(actual(str::normpath("foo/")) == "foo");
            wassert(actual(str::normpath("/foo")) == "/foo");
            wassert(actual(str::normpath("foo/bar")) == "foo/bar");
            wassert(actual(str::normpath("foo/./bar")) == "foo/bar");
            wassert(actual(str::normpath("././././foo/./././bar/././././")) == "foo/bar");
            wassert(actual(str::normpath("/../../../../../foo")) == "/foo");
            wassert(actual(str::normpath("foo/../foo/../foo/../foo/../")) == ".");
            wassert(actual(str::normpath("foo//bar")) == "foo/bar");
            wassert(actual(str::normpath("foo/./bar")) == "foo/bar");
            wassert(actual(str::normpath("foo/foo/../bar")) == "foo/bar");
        });

        add_method("split", []() {
            str::Split split("a/b//c", "/");
            auto a = split.begin();
            wassert(actual(a != split.end()).istrue());
            wassert(actual(*a) == "a");
            ++a;
            wassert(actual(a != split.end()).istrue());
            wassert(actual(*a) == "b");
            ++a;
            wassert(actual(a != split.end()).istrue());
            wassert(actual(*a) == "");
            ++a;
            wassert(actual(a != split.end()).istrue());
            wassert(actual(*a) == "c");
            ++a;
            wassert(actual(a == split.end()).istrue());

            vector<string> res;
            split = str::Split("std::string::", "::");
            std::copy(split.begin(), split.end(), std::back_inserter(res));
            wassert(actual(res.size()) == 3u);
            wassert(actual(res[0]) == "std");
            wassert(actual(res[1]) == "string");
            wassert(actual(res[2]) == "");

            res.clear();
            split = str::Split("", ",");
            std::copy(split.begin(), split.end(), std::back_inserter(res));
            wassert(actual(res.size()) == 0u);
        });

        add_method("split_noempty", []() {
            vector<string> res;
            str::Split split("a/b//c", "/", true);
            std::copy(split.begin(), split.end(), std::back_inserter(res));
            wassert(actual(res.size()) == 3u);
            wassert(actual(res[0]) == "a");
            wassert(actual(res[1]) == "b");
            wassert(actual(res[2]) == "c");

            res.clear();
            split = str::Split("///a/b/////c/", "/", true);
            std::copy(split.begin(), split.end(), std::back_inserter(res));
            wassert(actual(res.size()) == 3u);
            wassert(actual(res[0]) == "a");
            wassert(actual(res[1]) == "b");
            wassert(actual(res[2]) == "c");

            res.clear();
            split = str::Split("::std::::string::", "::", true);
            std::copy(split.begin(), split.end(), std::back_inserter(res));
            wassert(actual(res.size()) == 2u);
            wassert(actual(res[0]) == "std");
            wassert(actual(res[1]) == "string");
        });

        add_method("encode_cstring", []() {
            size_t len;
            wassert(actual(str::decode_cstring("cia\\x00o", len)) == string("cia\0o", 5));
            wassert(actual(len) == 8u);
            wassert(actual(str::encode_cstring(string("cia\0o", 5))) == "cia\\x00o");
        });

        add_method("encode_url", []() {
            wassert(actual(str::encode_url("")) == "");
            wassert(actual(str::encode_url("antani")) == "antani");
            wassert(actual(str::encode_url("a b c")) == "a%20b%20c");
            wassert(actual(str::encode_url("a ")) == "a%20");

            wassert(actual(str::decode_url("")) == "");
            wassert(actual(str::decode_url("antani")) == "antani");
            wassert(actual(str::decode_url("a%20b")) == "a b");
            wassert(actual(str::decode_url("a%20")) == "a ");
            wassert(actual(str::decode_url("a%2")) == "a");
            wassert(actual(str::decode_url("a%")) == "a");

            wassert(actual(str::decode_url(str::encode_url("àá☣☢☠!@#$%^&*(\")/A"))) == "àá☣☢☠!@#$%^&*(\")/A");
            wassert(actual(str::decode_url(str::encode_url("http://zz:ss@a.b:31/c?d=e&f=g"))) == "http://zz:ss@a.b:31/c?d=e&f=g");
        });

        add_method("encode_base64", []() {
            wassert(actual(str::encode_base64("")) == "");
            wassert(actual(str::encode_base64("antani")) == "YW50YW5p");
            wassert(actual(str::encode_base64("a b c")) == "YSBiIGM=");
            wassert(actual(str::encode_base64("a ")) == "YSA=");
            wassert(actual(str::encode_base64("àá☣☢☠!@#$%^&*(\")/A")) == "w6DDoeKYo+KYouKYoCFAIyQlXiYqKCIpL0E=");

            wassert(actual(str::decode_base64("")) == "");
            wassert(actual(str::decode_base64("YW50YW5p")) == "antani");
            wassert(actual(str::decode_base64("YSBi")) == "a b");
            wassert(actual(str::decode_base64("YSA=")) == "a ");
            wassert(actual(str::decode_base64("YQ==")) == "a");
            wassert(actual(str::decode_base64("w6DDoeKYo+KYouKYoCFAIyQlXiYqKCIpL0E=")) == "àá☣☢☠!@#$%^&*(\")/A");

            wassert(actual(str::decode_base64(str::encode_base64("àá☣☢☠!@#$%^&*(\")/A"))) == "àá☣☢☠!@#$%^&*(\")/A");
            wassert(actual(str::decode_base64(str::encode_base64("http://zz:ss@a.b:31/c?d=e&f=g"))) == "http://zz:ss@a.b:31/c?d=e&f=g");

            wassert(actual(str::encode_base64("")) == "");
            wassert(actual(str::encode_base64("c")) == "Yw==");
            wassert(actual(str::encode_base64("ci")) == "Y2k=");
            wassert(actual(str::encode_base64("cia")) == "Y2lh");
            wassert(actual(str::encode_base64("ciao")) == "Y2lhbw==");
            wassert(actual(str::encode_base64("ciao ")) == "Y2lhbyA=");
            wassert(actual(str::encode_base64("ciao c")) == "Y2lhbyBj");
            wassert(actual(str::encode_base64("ciao ci")) == "Y2lhbyBjaQ==");
            wassert(actual(str::encode_base64("ciao cia")) == "Y2lhbyBjaWE=");
            wassert(actual(str::encode_base64("ciao ciao")) == "Y2lhbyBjaWFv");

            wassert(actual(str::decode_base64(str::encode_base64(""))) == "");
            wassert(actual(str::decode_base64(str::encode_base64("c"))) == "c");
            wassert(actual(str::decode_base64(str::encode_base64("ci"))) == "ci");
            wassert(actual(str::decode_base64(str::encode_base64("cia"))) == "cia");
            wassert(actual(str::decode_base64(str::encode_base64("ciao"))) == "ciao");
            wassert(actual(str::decode_base64(str::encode_base64("ciao "))) == "ciao ");
            wassert(actual(str::decode_base64(str::encode_base64("ciao c"))) == "ciao c");
            wassert(actual(str::decode_base64(str::encode_base64("ciao ci"))) == "ciao ci");
            wassert(actual(str::decode_base64(str::encode_base64("ciao cia"))) == "ciao cia");
            wassert(actual(str::decode_base64(str::encode_base64("ciao ciao"))) == "ciao ciao");
        });
    }
} tests("string");

}
