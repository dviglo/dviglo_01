export module test.std_string_utils;

import "force_assert.h";

// Модули движка
import dviglo.std_string_utils;

using namespace std;

export void test_std_string_utils()
{
    {
        assert(replace_copy("aabbcc", "bb", "Q") == "aaQcc");
    }

    {
        string str = " www  ";
        trim_end(str);
        assert(str == " www");

        str = " www 123";
        trim_end(str, "2 31");
        assert(str == " www");
    }

    {
        assert(trim_copy("") == "");
        assert(trim_copy(" ") == "");
        assert(trim_copy(" ", "12") == " ");
        assert(trim_copy(" x 345 y ", " x") == "345 y");
    }

    {
        vector<string> result = split("", "");
        assert(result.size() == 1);
        assert(result[0] == "");

        result = split("", "", false);
        assert(result.size() == 0);

        result = split(" ", "");
        assert(result.size() == 1);
        assert(result[0] == " ");

        result = split(" ", "", false);
        assert(result.size() == 1);
        assert(result[0] == " ");

        result = split("  ", " ");
        assert(result.size() == 3);
        assert(result[0] == "");
        assert(result[1] == "");
        assert(result[2] == "");

        result = split("  ", " ", false);
        assert(result.size() == 0);

        result = split("qw 1", " 1");
        assert(result.size() == 2);
        assert(result[0] == "qw");
        assert(result[1] == "");

        result = split("qw 1", " 1", false);
        assert(result.size() == 1);
        assert(result[0] == "qw");

        result = split("qw  asd", " ", false);
        assert(result.size() == 2);
        assert(result[0] == "qw");
        assert(result[1] == "asd");
    }
}
