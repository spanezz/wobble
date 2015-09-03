#include <wobble/string.h>
#include <vector>

using namespace std;

namespace wobble {
namespace str {

std::string basename(const std::string& pathname)
{
    size_t pos = pathname.rfind("/");
    if (pos == std::string::npos)
        return pathname;
    else
        return pathname.substr(pos+1);
}

std::string dirname(const std::string& pathname)
{
    size_t pos = pathname.rfind("/");
    if (pos == std::string::npos)
        return std::string();
    else if (pos == 0)
        // Handle the case of '/foo'
        return std::string("/");
    else
        return pathname.substr(0, pos);
}

void appendpath(std::string& dest, const std::string& path2)
{
    if (path2.empty())
        return;

    if (dest.empty())
    {
        dest = path2;
        return;
    }

    if (dest[dest.size() - 1] == '/')
        if (path2[0] == '/')
            dest += path2.substr(1);
        else
            dest += path2;
    else
        if (path2[0] == '/')
            dest += path2;
        else
        {
            dest += '/';
            dest += path2;
        }
}

std::string joinpath(const std::string& path1, const std::string& path2)
{
    string res = path1;
    appendpath(res, path2);
    return res;
}

std::string normpath(const std::string& pathname)
{
    vector<string> st;
    if (pathname[0] == '/')
        st.push_back("/");

    split(pathname, '/', [&st](const std::string& i) {
        if (i == "." || i.empty()) return;;
        if (i == "..")
            if (st.back() == "..")
                st.emplace_back(i);
            else if (st.back() == "/")
                return;
            else
                st.pop_back();
        else
            st.emplace_back(i);
    });

    if (st.empty())
        return ".";

    string res;
    for (const auto& i: st)
        appendpath(res, i);
    return res;
}

std::string encode_cstring(const std::string& str)
{
    string res;
    for (string::const_iterator i = str.begin(); i != str.end(); ++i)
        if (*i == '\n')
            res += "\\n";
        else if (*i == '\t')
            res += "\\t";
        else if (*i == 0 || iscntrl(*i))
        {
            char buf[5];
            snprintf(buf, 5, "\\x%02x", (unsigned int)*i);
            res += buf;
        }
        else if (*i == '"' || *i == '\\')
        {
            res += "\\";
            res += *i;
        }
        else
            res += *i;
    return res;
}

std::string decode_cstring(const std::string& str, size_t& lenParsed)
{
    string res;
    string::const_iterator i = str.begin();
    for ( ; i != str.end() && *i != '"'; ++i)
        if (*i == '\\' && (i+1) != str.end())
        {
            switch (*(i+1))
            {
                case 'n': res += '\n'; break;
                case 't': res += '\t'; break;
                case 'x': {
                              size_t j;
                              char buf[5] = "0x\0\0";
                              // Read up to 2 extra hex digits
                              for (j = 0; j < 2 && i+2+j != str.end() && isxdigit(*(i+2+j)); ++j)
                                  buf[2+j] = *(i+2+j);
                              i += j;
                              res += (char)atoi(buf);
                              break;
                          }
                default:
                          res += *(i+1);
                          break;
            }
            ++i;
        } else
            res += *i;
    if (i != str.end() && *i == '"')
        ++i;
    lenParsed = i - str.begin();
    return res;
}

std::string encode_url(const std::string& str)
{
    string res;
    for (string::const_iterator i = str.begin(); i != str.end(); ++i)
    {
        if ( (*i >= '0' && *i <= '9') || (*i >= 'A' && *i <= 'Z')
          || (*i >= 'a' && *i <= 'z') || *i == '-' || *i == '_'
          || *i == '!' || *i == '*' || *i == '\'' || *i == '(' || *i == ')')
            res += *i;
        else {
            char buf[4];
            snprintf(buf, 4, "%%%02x", static_cast<unsigned>(static_cast<unsigned char>(*i)));
            res += buf;
        }
    }
    return res;
}

std::string decode_url(const std::string& str)
{
    string res;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (str[i] == '%')
        {
            // If there's a partial %something at the end, ignore it
            if (i >= str.size() - 2)
                return res;
            res += static_cast<char>(strtoul(str.substr(i+1, 2).c_str(), 0, 16));
            i += 2;
        }
        else
            res += str[i];
    }
    return res;
}

static const char* base64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

template<typename T>
static const char invbase64(const T& idx)
{
    static const char data[] = {62,0,0,0,63,52,53,54,55,56,57,58,59,60,61,0,0,0,0,0,0,0,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,0,0,0,0,0,0,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51};
    if (idx < 43) return 0;
    if (static_cast<unsigned>(idx) > 43 + (sizeof(data)/sizeof(data[0]))) return 0;
    return data[idx - 43];
}

std::string encode_base64(const std::string& str)
{
    std::string res;

    for (size_t i = 0; i < str.size(); i += 3)
    {
        // Pack every triplet into 24 bits
        unsigned int enc;
        if (i + 3 < str.size())
            enc = ((unsigned char)str[i] << 16) | ((unsigned char)str[i + 1] << 8) | (unsigned char)str[i + 2];
        else
        {
            enc = ((unsigned char)str[i] << 16);
            if (i + 1 < str.size())
                enc |= (unsigned char)str[i + 1] << 8;
            if (i + 2 < str.size())
                enc |= (unsigned char)str[i + 2];
        }

        // Divide in 4 6-bit values and use them as indexes in the base64 char
        // array
        for (int j = 18; j >= 0; j -= 6)
            res += base64[(enc >> j) & 63];
    }

    // Replace padding characters with '='
    if (str.size() % 3)
        for (size_t i = 0; i < 3 - (str.size() % 3); ++i)
            res[res.size() - i - 1] = '=';

    return res;
}

std::string decode_base64(const std::string& str)
{
    std::string res;

    for (size_t i = 0; i < str.size(); i += 4)
    {
        // Pack every quadruplet into 24 bits
        unsigned int enc;
        if (i+4 < str.size())
        {
            enc = (invbase64(str[i]) << 18)
                + (invbase64(str[i+1]) << 12)
                + (invbase64(str[i+2]) << 6)
                + (invbase64(str[i+3]));
        } else {
            enc = (invbase64(str[i]) << 18);
            if (i+1 < str.size())
                enc += (invbase64(str[i+1]) << 12);
            if (i+2 < str.size())
                enc += (invbase64(str[i+2]) << 6);
            if (i+3 < str.size())
                enc += (invbase64(str[i+3]));
        }

        // Divide in 3 8-bit values and append them to the result
        res += enc >> 16 & 0xff;
        res += enc >> 8 & 0xff;
        res += enc & 0xff;
    }

    // Remove trailing padding
    if (str.size() > 0)
        for (size_t i = str.size() - 1; str[i] == '='; --i)
        {
            if (res.size() > 0)
                res.resize(res.size() - 1);
            if (i == 0 || res.size() == 0 )
                break;
        }

    return res;
}


}
}
