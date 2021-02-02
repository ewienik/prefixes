
/**/

#include "mem.hpp"
#include "prefixes.hpp"

/**/

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

/**/

using namespace std;

/**/

namespace {

/**/

string test_title;  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
int test_count{};   // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)
int test_failed{};  // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)

/**/

void TestStarted(char const* title) {
    test_title = string(title);
    test_count = 0;
    test_failed = 0;
    cout << "STARTED: " << test_title << endl;
}

/**/

void TestFinished() { cout << "FINISHED: " << test_title << " (" << test_count << ", " << test_failed << ")" << endl; }

/**/

void TestMessage(string const& msg) { cout << "MESSAGE: " << test_title << " " << msg << endl; }

/**/

void Test(Prefixes& prefixes, int ip, int expected) {
    test_count++;
    int result = prefixes.Check(ip);
    if (result != expected) {
        test_failed++;
        cout << "FAILED: " << test_title << ": " << setw(8) << setfill('0') << hex << ip << setw(0) << dec << ", "
             << static_cast<int>(expected) << " != " << static_cast<int>(result) << endl;
    }
}

/**/

void TestMem() {
    TestStarted(static_cast<char const*>(__FUNCTION__));

    Mem<int, 2> mem;

    int* first = static_cast<int*>(mem.Malloc());
    if (nullptr == first) { cout << "FAILED: " << test_title << ": allocation of first int should be != NULL" << endl; }
    int* second = static_cast<int*>(mem.Malloc());
    if (nullptr == second) {
        cout << "FAILED: " << test_title << ": allocation of second int should be != NULL" << endl;
    }

    mem.Free(second);
    int* third = static_cast<int*>(mem.Malloc());
    if (nullptr == third) { cout << "FAILED: " << test_title << ": allocation of third int should be != NULL" << endl; }
    if (third != second) {
        cout << "FAILED: " << test_title << ": allocation of third int should be as second" << endl;
    }

    int* fourth = static_cast<int*>(mem.Malloc());
    if (nullptr == fourth) {
        cout << "FAILED: " << test_title << ": allocation of fourth int should be != NULL" << endl;
    }
    if (fourth == second) {
        cout << "FAILED: " << test_title << ": allocation of fourth int should be != second" << endl;
    }

    TestFinished();
}

void Test1() {
    TestStarted(static_cast<char const*>(__FUNCTION__));

    Prefixes prefixes;

    prefixes.Add(0x0a000000, 8);
    prefixes.Add(0x0a000000, 16);
    prefixes.Add(0x0b040201, 32);

    Test(prefixes, 0x0a000000, 16);
    Test(prefixes, 0x0a200000, 8);
    Test(prefixes, 0x0b200000, -1);
    Test(prefixes, 0x0b040201, 32);

    prefixes.Add(0x0a000000, 16);

    Test(prefixes, 0x0a000000, 16);
    Test(prefixes, 0x0a200000, 8);
    Test(prefixes, 0x0b200000, -1);
    Test(prefixes, 0x0b040201, 32);

    prefixes.Add(0x0a000000, 0);

    Test(prefixes, 0x0a000000, 16);
    Test(prefixes, 0x0a200000, 8);
    Test(prefixes, 0x0b200000, 0);
    Test(prefixes, 0x0b040201, 32);

    prefixes.Del(0x0a000000, 16);

    Test(prefixes, 0x0a000000, 8);
    Test(prefixes, 0x0a200000, 8);
    Test(prefixes, 0x0b200000, 0);
    Test(prefixes, 0x0b040201, 32);

    prefixes.Del(0x00040000, 0);

    Test(prefixes, 0x0a000000, 8);
    Test(prefixes, 0x0a200000, 8);
    Test(prefixes, 0x0b200000, -1);
    Test(prefixes, 0x0b040201, 32);

    TestFinished();
}

/**/

auto Ip(int i, int j, int k, int l) -> int { return i << 24 | j << 16 | k << 8 | l; }

/**/

void Test2() {
    TestStarted(static_cast<char const*>(__FUNCTION__));

    Prefixes prefixes;

    for (int t = 0; t < 10; t++) {
        {
            ostringstream ostr;
            ostr << "Loop " << t << " Phase 1";
            TestMessage(ostr.str());
        }

        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                for (int k = t * 10; k < (t + 1) * 10; k++) {
                    for (int l = t * 10; l < (t + 1) * 10; l++) {
                        for (int m = 0; m < 33; m++) { prefixes.Add(Ip(i, j, k, l), m); }
                    }
                }
            }
        }

        {
            ostringstream ostr;
            ostr << "Loop " << t << " Phase 2";
            TestMessage(ostr.str());
        }

        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                for (int k = t * 10; k < (t + 1) * 10; k++) {
                    for (int l = t * 10; l < (t + 1) * 10; l++) {
                        int ip = Ip(i, j, k, l);
                        Test(prefixes, ip, 32);
                    }
                }
            }
        }

        {
            ostringstream ostr;
            ostr << "Loop " << t << " Phase 3";
            TestMessage(ostr.str());
        }

        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                for (int k = t * 10; k < (t + 1) * 10; k++) {
                    for (int l = t * 10; l < (t + 1) * 10; l++) {
                        for (int m = 0; m < 33; m++) { prefixes.Del(Ip(i, j, k, l), m); }
                    }
                }
            }
        }

        {
            ostringstream ostr;
            ostr << "Loop " << t << " Phase 4";
            TestMessage(ostr.str());
        }

        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                for (int k = 0; k < 10; k++) {
                    for (int l = 0; l < 10; l++) {
                        int ip = Ip(i, j, k, l);
                        Test(prefixes, ip, -1);
                    }
                }
            }
        }
    }

    TestFinished();
}

/**/

}  // namespace

/**/

auto main() -> int {
    TestMem();
    Test1();
    Test2();
}

/**/

