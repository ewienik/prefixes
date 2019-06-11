
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

string test_title;
int test_count;
int test_failed;

/**/

void TestStarted(string title) {
    test_title = title;
    test_count = 0;
    test_failed = 0;
    cout << "STARTED: " << test_title << endl;
}

/**/

void TestFinished() {
    cout << "FINISHED: " << test_title << " (" << test_count << ", "
         << test_failed << ")" << endl;
}

/**/

void TestMessage(string msg) {
    cout << "MESSAGE: " << test_title << " " << msg << endl;
}

/**/

void Test(Prefixes& prefixes, int ip, char expected) {
    test_count++;
    char result = prefixes.Check(ip);
    if (result != expected) {
        test_failed++;
        cout << "FAILED: " << test_title << ": " << setw(8) << setfill('0')
             << hex << ip << setw(0) << dec << ", "
             << static_cast<int>(expected) << " != " << static_cast<int>(result)
             << endl;
    }
}

/**/

void TestMem() {
    TestStarted(__FUNCTION__);

    Mem<int, 2> mem;

    int* first = static_cast<int*>(mem.Malloc());
    if (0 == first) {
        cout << "FAILED: " << test_title
             << ": allocation of first int should be != NULL" << endl;
    }
    int* second = static_cast<int*>(mem.Malloc());
    if (0 == second) {
        cout << "FAILED: " << test_title
             << ": allocation of second int should be != NULL" << endl;
    }

    mem.Free(second);
    int* third = static_cast<int*>(mem.Malloc());
    if (0 == third) {
        cout << "FAILED: " << test_title
             << ": allocation of third int should be != NULL" << endl;
    }
    if (third != second) {
        cout << "FAILED: " << test_title
             << ": allocation of third int should be as second" << endl;
    }

    mem.Free(reinterpret_cast<char*>(second) + 1);
    int* fourth = static_cast<int*>(mem.Malloc());
    if (0 == fourth) {
        cout << "FAILED: " << test_title
             << ": allocation of fourth int should be != NULL" << endl;
    }
    if (fourth == second) {
        cout << "FAILED: " << test_title
             << ": allocation of fourth int should be != second" << endl;
    }

    TestFinished();
}

void Test1() {
    TestStarted(__FUNCTION__);

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

int Ip(int i, int j, int k, int l) { return i << 24 | j << 16 | k << 8 | l; }

/**/

void Test2() {
    TestStarted(__FUNCTION__);

    Prefixes prefixes;

    for (int t = 0; t < 10; t++) {
        ostringstream ostr;
        ostr << "Loop " << t << "...";
        TestMessage(ostr.str());

        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                for (int k = t * 10; k < (t + 1) * 10; k++) {
                    for (int l = t * 10; l < (t + 1) * 10; l++) {
                        for (int m = 0; m < 33; m++) {
                            prefixes.Add(Ip(i, j, k, l), m);
                        }
                    }
                }
            }
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

        for (int i = 0; i < 100; i++) {
            for (int j = 0; j < 100; j++) {
                for (int k = t * 10; k < (t + 1) * 10; k++) {
                    for (int l = t * 10; l < (t + 1) * 10; l++) {
                        for (int m = 0; m < 33; m++) {
                            prefixes.Del(Ip(i, j, k, l), m);
                        }
                    }
                }
            }
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

int main() {
    TestMem();
    Test1();
    Test2();
}

/**/

