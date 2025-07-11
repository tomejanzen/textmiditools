#include <cfenv>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>

#include "RhythmRational.h"

using std::cout, std::cerr, std::istringstream, std::boolalpha, std::ifstream,
      std::setprecision, std::noskipws;
using std::runtime_error, std::string, std::numeric_limits, std::unique_ptr;

using textmidi::rational::RhythmRational, textmidi::rational::print_rhythm,
      textmidi::rational::PrintRhythmSimpleContinuedFraction,
      textmidi::rational::SimpleContinuedFraction;

using std::vector;

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
    SimpleContinuedFraction scf{{3, {7, 15, 1, 292}}};
    cout << "a SimpleContinuedFraction " << scf << '\n';
    RhythmRational pi{scf};
    cout << setprecision(24) << (static_cast<double>(pi)) << '\n';
    istringstream iss8{"[3;7,15,1,292]"};
    SimpleContinuedFraction scf2{};

    cout << "input:\n";
    iss8 >> scf2;
    cout << scf2 << '\n';

    RhythmRational rrscf{scf2};
    cout << static_cast<double>(rrscf) << '\n';

    string str;
#if 0
    SimpleContinuedFraction scf3;
    cout << "ENTER A SIMPLE CONTINUED FRACTION: ";
    cin >> scf3;
    RhythmRational rrscf3{scf3};
    cout << setprecision(24) << static_cast<double>(rrscf3) << '\n';

    auto scf3b{static_cast<SimpleContinuedFraction>(rrscf3)};
    cout << scf3b << '\n';
#endif
    RhythmRational zero{0L, 1L};
    RhythmRational one{1L, 1L};
    cout << "zero times one: " << (zero *= one) << '\n';

    istringstream iss11{"[4]"};
    SimpleContinuedFraction scf11{};
    iss11 >> scf11;
    cout << "Integer only: " << ' ' << scf11 << '\n';

    textmidi::rational::print_rhythm
        .reset(new PrintRhythmSimpleContinuedFraction);


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

    cout << "from_double:\n";
    RhythmRational d1{};
    d1.from_double(1.5, 2L, true);
    cout << "1.5: " << d1 << '\n';

    RhythmRational d2{};
    d2.from_double(11.667, 3L, true);
    cout << "11.667: " << d2 << '\n';

    cout << "numeric_limits:\n";
    cout << "max: " << numeric_limits<RhythmRational>::max() << '\n';
    cout << "min: " << numeric_limits<RhythmRational>::min() << '\n';
    cout << "lowest: " << numeric_limits<RhythmRational>::lowest() << '\n';
    cout << "epsilon: " << numeric_limits<RhythmRational>::epsilon() << '\n';
    cout << "round_error: " << numeric_limits<RhythmRational>::round_error() << '\n';
    cout << "infinity: " << numeric_limits<RhythmRational>::infinity() << '\n';
    cout << "quiet_NaN: " << numeric_limits<RhythmRational>::quiet_NaN() << '\n';
    cout << "signaling_NaN: " << numeric_limits<RhythmRational>::signaling_NaN() << '\n';
    cout << "denorm_min: " << numeric_limits<RhythmRational>::denorm_min() << '\n';
    cout << "is_specialized: " << numeric_limits<RhythmRational>::is_specialized << '\n';
    cout << "is_signed: " << numeric_limits<RhythmRational>::is_signed << '\n';
    cout << "is_integer: " << numeric_limits<RhythmRational>::is_integer << '\n';
    cout << "has_infinity: " << numeric_limits<RhythmRational>::has_infinity << '\n';
    cout << "has_quiet_NaN: " << numeric_limits<RhythmRational>::has_quiet_NaN << '\n';
    cout << "has_signaling_NaN: " << numeric_limits<RhythmRational>::has_signaling_NaN << '\n';
    cout << "has_denorm_loss: " << numeric_limits<RhythmRational>::has_denorm_loss << '\n';
    cout << "round_style: " << numeric_limits<RhythmRational>::round_style << '\n';
    cout << "is_iec559: " << numeric_limits<RhythmRational>::is_iec559 << '\n';
    cout << "is_bounded: " << numeric_limits<RhythmRational>::is_bounded << '\n';
    cout << "is_modulo: " << numeric_limits<RhythmRational>::is_modulo << '\n';
    cout << "digits: " << numeric_limits<RhythmRational>::digits << '\n';
    cout << "digits10: " << numeric_limits<RhythmRational>::digits10 << '\n';
    cout << "max_digits10: " << numeric_limits<RhythmRational>::max_digits10 << '\n';
    cout << "radix: " << numeric_limits<RhythmRational>::radix << '\n';
    cout << "min_exponent: " << numeric_limits<RhythmRational>::min_exponent << '\n';
    cout << "max_exponent: " << numeric_limits<RhythmRational>::max_exponent << '\n';
    cout << "traps: " << numeric_limits<RhythmRational>::traps << '\n';
    cout << "tinyness_before: " << numeric_limits<RhythmRational>::tinyness_before << '\n';
    cout << "prime reciprocals:\n";
    vector<RhythmRational> rps{RhythmRational(1L, 2L), RhythmRational(1L, 3L), 
        RhythmRational(1L, 5L), RhythmRational(1L, 7L), RhythmRational(1L, 11L), 
        RhythmRational(1L, 13L), RhythmRational(1L, 17L), RhythmRational(1L, 19L), 
        RhythmRational(1L, 23L), RhythmRational(1L, 29L), RhythmRational(1L, 31L), 
        RhythmRational(1L, 37L), RhythmRational(1L, 41L), RhythmRational(1L, 43L)};
    for (const auto& rp : rps)
    {
        (*print_rhythm)(cout, rp);
        cout << '\n';
    }

    std::map<int, string> roundmap
    {
        {FE_TOWARDZERO, "FE_TOWARDZERO"},
        {FE_DOWNWARD, "FE_DOWNWARD"},
        {FE_UPWARD, "FE_UPWARD"},
        {FE_TONEAREST,"FE_TONEAREST" }
    };
    cout << "FLT_ROUNDS: " << roundmap[fegetround()] << '\n';
    const RhythmRational snapper{1L, 4L};
    for (RhythmRational q3{0L}; q3 < RhythmRational{2L}; q3 += RhythmRational{1L, 8L})
    {
        cout << q3 << " rounded: " << q3.round() << " snap to " << snapper << ' '<< q3.snap(snapper) << '\n';
    }
    for (RhythmRational q4{0L}; q4 < RhythmRational{2L}; q4 += RhythmRational{1L, 7L})
    {
        cout << q4 << " rounded: " << q4.round() << " snap to " << snapper << ' '<< q4.snap(snapper) << '\n';
    }
    for (RhythmRational q4{2L}; q4 > RhythmRational{-2L}; q4 -= RhythmRational{1L, 7L})
    {
        cout << q4 << " rounded: " << q4.round() << " snap to " << snapper << ' '<< q4.snap(snapper) << '\n';
    }
    fesetround(FE_TOWARDZERO);
    cout << "FE_TOWARDZERO ";
    cout << "FLT_ROUNDS: " << roundmap[fegetround()] << '\n';
    for (RhythmRational q4{2L}; q4 > RhythmRational{-2L}; q4 -= RhythmRational{1L, 7L})
    {
        cout << q4 << " rounded: " << q4.round() << " snap to " << snapper << ' '<< q4.snap(snapper) << '\n';
    }
    fesetround(FE_DOWNWARD);
    cout << "FE_DOWNWARD ";
    cout << "FLT_ROUNDS: " << roundmap[fegetround()] << '\n';
    for (RhythmRational q4{2L}; q4 > RhythmRational{-2L}; q4 -= RhythmRational{1L, 7L})
    {
        cout << q4 << " rounded: " << q4.round() << " snap to " << snapper << ' '<< q4.snap(snapper) << '\n';
    }
    fesetround(FE_UPWARD);
    cout << "FE_UPWARD ";
    cout << "FLT_ROUNDS: " << roundmap[fegetround()] << '\n';
    for (RhythmRational q4{2L}; q4 > RhythmRational{-2L}; q4 -= RhythmRational{1L, 7L})
    {
        cout << q4 << " rounded: " << q4.round() << " snap to " << snapper << ' '<< q4.snap(snapper) << '\n';
    }
    fesetround(FE_TONEAREST);
    cout << "FE_TONEAREST ";
    cout << "FLT_ROUNDS: " << roundmap[fegetround()] << '\n';
    for (RhythmRational q4{2L}; q4 > RhythmRational{-2L}; q4 -= RhythmRational{1L, 7L})
    {
        cout << q4 << " rounded: " << q4.round() << " snap to " << snapper << ' '<< q4.snap(snapper) << '\n';
    }
    RhythmRational q1{2L, 3L};
    RhythmRational q2{1L, 2L};
    return 0;
}

