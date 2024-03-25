#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "RhythmRational.h"

using namespace std;

using namespace textmidi;
using namespace textmidi;
using namespace textmidi::rational;

int main()
{
    RhythmRational r{1, 2};
    RhythmRational s{2, 3};
    RhythmRational t{r * s};
    cout << "OPERATORS:\n";
    cout << "r: " << r << " s: "  << s << " t: " << t << '\n';
    auto u{r + s};
    cout << "u: " << u << '\n';
    auto v{s - r};
    cout << "s - r: " << v << '\n';
    auto w{s / r};
    cout << "s / r: " << w << '\n';
    cout << "(r * r) " << (r * r) << '\n';
    cout << "(r / r) " << (r / r) << '\n';
    cout << "r == r " << boolalpha << (r == r) << '\n';
    cout << "r != r " << boolalpha << (r != r) << '\n';
    cout << "s < r " << boolalpha << (s < r) << '\n';
    cout << "r > s " << boolalpha << (r > s) << '\n';
    cout << " r <= r " << boolalpha << (r <= r) << '\n';
    cout << " r <= s " << boolalpha << (r <= s) << '\n';
    cout << " r >= r " << boolalpha << (r >= r) << '\n';
    cout << "r >= s " << boolalpha << (r >= s) << '\n';
    RhythmRational x{4104, 3672};
    x.reduce();
    cout << "reduce 4104/3672 " << x << '\n';

    r += s;
    cout << "r += s: " << r << '\n';
    r -= s;
    cout << "r -= s: " << r << '\n';
    r /= s;
    cout << "r /= s: " << r << '\n';
    r *= s;
    cout << "r *= s: " << r << '\n';
    
    RhythmRational y{-1, 4};
    cout << "y " << y << " abs(y) " << abs(y) << '\n';

    RhythmRational z{2910L, 2670L};
    cout << "2910 / 2670 " << z << '\n';
    try
    {
        RhythmRational a{0L, 0L};
    }
    catch (std::exception& ex)
    {
        cerr << ex.what() << '\n';
    }
    cout << "musical rhythm printing:\n";
    RhythmRational c{3L, 2L},
                     d{3L, 4L},
                     e{3L, 8L},
                     f{3L, 16L},
                     g{1L, 4L},
                     h{3L, 4L},
                     i{7L, 4L},
                     j{7L, 3L},
                     k{3L, 5L},
                     p{32L, 4L},
                     q{-16L, -4L},
                     ww{8L, -4L};
    cout << "PRINT AS RATIONAL THEN AS RHYTHM\n";
    cout << "c: " << c << ' ';
    (*print_rhythm)(cout, c);
    cout << '\n';
    cout << "d: " << d << ' ';
    (*print_rhythm)(cout, d);
    cout << '\n';
    cout << "e: " << e << ' ';
    (*print_rhythm)(cout, e);
    cout << '\n';
    cout << "f: " << f << ' ';
    (*print_rhythm)(cout, f);
    cout << '\n';
    cout << "g: " << g << ' ';
    (*print_rhythm)(cout, g);
    cout << '\n';
    cout << "h: " << h << ' ';
    (*print_rhythm)(cout, h);
    cout << '\n';
    cout << "i: " << i << ' ';
    (*print_rhythm)(cout, i);
    cout << '\n';
    cout << "j: " << j << ' ';
    (*print_rhythm)(cout, j);
    cout << '\n';
    cout << "k: " << k << ' ';
    (*print_rhythm)(cout, k);
    cout << '\n';

    RhythmRational m{32, 32};
    cout << "m: " << m << ' ';
    (*print_rhythm)(cout, m);
    cout << '\n';
    cout << "p: " << p << ' ';
    (*print_rhythm)(cout, p);
    cout << '\n';
    cout << "q: " << q << ' ';
    (*print_rhythm)(cout, q);
    cout << '\n';
    cout << "ww: " << ww << ' ';
    (*print_rhythm)(cout, ww);
    cout << '\n';

    RhythmRational n{1L, 1L};
    cout << "n: " << n << ' ';
    (*print_rhythm)(cout, n);
    cout << '\n';

    cout << "STRING STREAM:\n";
    istringstream iss("-6/-8");
    RhythmRational anum{};
    iss >> anum;
    cout << "RESULT: : " << anum << '\n';


    istringstream iss2("9223372036854775807/-9223372036854775807");
    RhythmRational bnum{};
    iss2 >> bnum;
    cout << "RESULT: : " << bnum << '\n';


    istringstream iss3("1/9223372036854775807");
    RhythmRational cnum{};
    iss3 >> cnum;
    cout << "RESULT: : " << cnum << '\n';

#if 1
    istringstream iss4("1/9 3.14159265358979 Z");
    RhythmRational drat{};
    double thepi{};
    char ch{'A'};
    iss4 >> drat >> thepi >> ch;
    cout << "RESULT: : " << drat << ' ' << thepi << ' ' << ch << '\n';
#else
    istringstream iss4("1/9 3.14159265358979 Z");
    RhythmRational drat{};
    string sos;
    iss4 >> drat >> sos;
    cout << "RESULT: : " << drat << ' ' << sos << '\n';
#endif

    istringstream iss5("@/9 3.14159265358979 Z");
    RhythmRational dratb{};
    double thepib{};
    char chb{'A'};
    try
    {
        iss5 >> dratb >> thepib >> chb;
    }
    catch (runtime_error& er)
    {
        cerr << er.what() << '\n';
    }
    cout << "RESULT: : " << dratb << ' ' << thepib << ' ' << chb << '\n';

    cout << "FILE STREAM:\n";

    ifstream ifs{"testRhythmRational.dat"};
    RhythmRational rat{};
    string name{};
    double val{};
    string str1{};
    string str2{};
    string str3{};

    getline(ifs, str1);
    cout << "STRING: " << str1 << '\n';
    istringstream iss6(str1);

    iss6 >> name >> rat >> val;
    cout << "RESULT: " << name << ' ' << rat << ' ' << val << '\n';
    iss6.clear();
    getline(ifs, str2);
    cout << "STRING: " << str2 << '\n';
    iss6.str(str2);
    iss6 >> rat >> val >> name;
    cout << "RESULT: " << rat << ' ' << val << ' ' << name << ' ' << '\n';
    iss6.clear();
    getline(ifs, str3);
    cout << "STRING: " << str3 << '\n';
    iss6.str(str3);
    iss6 >> val >> name >> rat >> noskipws;
    cout << "RESULT: " << val << ' ' << name << ' ' << rat << '\n';

    // Pi as a simple continued fraction.
    //
    // 3 + 1
    //     ------------------
    //     7 + 1
    //         -------
    //         15 + 1
    //              ----
    //              1 + 1
    //                  ---
    //                  292, 1, 1, 1, 2, 1, 3, 1, 14, 2, 1, 1, 2, 2, 2, 2, 1, 84, 2, 1, 1, 15, 3, 13, 1, 4
    // [3; 7, 15, 1, 292, 1, 1, 1, 2, 1, 3, 1, 14, 2, 1, 1, 2, 2, 2, 2, 1, 84, 2, 1, 1, 15, 3, 13, 1, 4]
    //
    cout << RhythmRational{3} + RhythmRational{1} 
                                / (RhythmRational{7} + RhythmRational{1} 
                                                       / (RhythmRational{15} + RhythmRational{1} 
                                                                               / (RhythmRational{1} + (RhythmRational{1} 
                                                                                                      / RhythmRational{292})))) << '\n';
    RhythmRational::SimpleContinuedFraction scf{3, {7, 15, 1, 292}};
    cout << scf << '\n';
    RhythmRational pi{scf};
    cout << setprecision(24) << (static_cast<double>(pi)) << '\n'; 
    istringstream iss8{"[3;7,15,1,292]"};
    RhythmRational::SimpleContinuedFraction scf2{};

    cout << "input:\n";
    iss8 >> scf2;
    cout << scf2 << '\n';
    
    RhythmRational rrscf{scf2};
    cout << static_cast<double>(rrscf) << '\n';

    string str;
    RhythmRational::SimpleContinuedFraction scf3;
    cout << "ENTER A SIMPLE CONTINUED FRACTION: ";
    cin >> scf3;
    RhythmRational rrscf3{scf3};
    cout << setprecision(24) << static_cast<double>(rrscf3) << '\n';

    auto scf3b{static_cast<RhythmRational::SimpleContinuedFraction>(rrscf3)};
    cout << scf3b << '\n';

    RhythmRational zero{0L, 1L};
    RhythmRational one{1L, 1L};
    cout << "zero times one: " << (zero *= one) << '\n';

    istringstream iss11{"[4]"};
    RhythmRational::SimpleContinuedFraction scf11{};
    iss11 >> scf11;
    cout << "Integer only: " << ' ' << scf11 << '\n';

    textmidi::rational::print_rhythm.reset(new PrintRhythmSimpleContinuedFraction);

    cout << "PRINT AS SIMPLE CONTINUED FRACTION\n";
    cout << "c: " << c << ' ';
    (*print_rhythm)(cout, c);
    cout << '\n';
    cout << "d: " << d << ' ';
    (*print_rhythm)(cout, d);
    cout << '\n';
    cout << "e: " << e << ' ';
    (*print_rhythm)(cout, e);
    cout << '\n';
    cout << "f: " << f << ' ';
    (*print_rhythm)(cout, f);
    cout << '\n';
    cout << "g: " << g << ' ';
    (*print_rhythm)(cout, g);
    cout << '\n';
    cout << "h: " << h << ' ';
    (*print_rhythm)(cout, h);
    cout << '\n';
    cout << "i: " << i << ' ';
    (*print_rhythm)(cout, i);
    cout << '\n';
    cout << "j: " << j << ' ';
    (*print_rhythm)(cout, j);
    cout << '\n';
    cout << "k: " << k << ' ';
    (*print_rhythm)(cout, k);
    cout << '\n';

    return 0;
}

