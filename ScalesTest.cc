
#include <iostream>
#include <algorithm>
#include <vector>
#include "Scales.h"

using namespace std;
using namespace cgm;

int main(int argc, char *argv[])
{
    vector<string> keynames(scale_strs.size(), " END_LAZY KEY_SIGNATURE C LAZY ");
    keynames[1] = " END_LAZY KEY_SIGNATURE C# LAZY ";
    keynames[2] = " END_LAZY KEY_SIGNATURE D LAZY ";
    keynames[3] = " END_LAZY KEY_SIGNATURE D# LAZY ";
    keynames[4] = " END_LAZY KEY_SIGNATURE E LAZY ";
    keynames[5] = " END_LAZY KEY_SIGNATURE F LAZY ";
    keynames[6] = " END_LAZY KEY_SIGNATURE F# LAZY ";
    keynames[7] = " END_LAZY KEY_SIGNATURE G LAZY ";
    keynames[8] = " END_LAZY KEY_SIGNATURE Ab LAZY ";
    keynames[9] = " END_LAZY KEY_SIGNATURE A LAZY ";
    keynames[10] = " END_LAZY KEY_SIGNATURE Bb LAZY ";
    keynames[11] = " END_LAZY KEY_SIGNATURE B LAZY ";

    keynames[12] = " END_LAZY KEY_SIGNATURE c LAZY ";
    keynames[13] = " END_LAZY KEY_SIGNATURE c# LAZY ";
    keynames[14] = " END_LAZY KEY_SIGNATURE d LAZY ";
    keynames[15] = " END_LAZY KEY_SIGNATURE d# LAZY ";
    keynames[16] = " END_LAZY KEY_SIGNATURE e LAZY ";
    keynames[17] = " END_LAZY KEY_SIGNATURE f LAZY ";
    keynames[18] = " END_LAZY KEY_SIGNATURE f# LAZY ";
    keynames[19] = " END_LAZY KEY_SIGNATURE g LAZY ";
    keynames[20] = " END_LAZY KEY_SIGNATURE ab LAZY ";
    keynames[21] = " END_LAZY KEY_SIGNATURE a LAZY ";
    keynames[22] = " END_LAZY KEY_SIGNATURE bb LAZY ";
    keynames[23] = " END_LAZY KEY_SIGNATURE b LAZY ";
    keynames[24] = " END_LAZY KEY_SIGNATURE G LAZY ";
    keynames[30] = " END_LAZY KEY_SIGNATURE F LAZY ";
    keynames[31] = " END_LAZY KEY_SIGNATURE B LAZY ";
    keynames[33] = " END_LAZY KEY_SIGNATURE B LAZY ";
    keynames[34] = " END_LAZY KEY_SIGNATURE G LAZY ";
    keynames[36] = " END_LAZY KEY_SIGNATURE B LAZY ";
    keynames[37] = " END_LAZY KEY_SIGNATURE D LAZY ";

    cout << "FILEHEADER 1 240\n";

    cout << "STARTTRACK\n";
    cout << "TIME_SIGNATURE 4 4 240\n";
    cout << "TEMPO 240\n";
    cout << "TEXT CREATED BY TEXTMIDICGM BY THOMAS JANZEN\n";
    cout << "TRACK Scales\n";
    cout << "LAZY\n";
 
    cout << ";" << scale_strs.size() << '\n';
    for (auto ctr{0}; ctr < scale_strs.size(); ++ctr)
    {
        auto scale(scale_strs[ctr]);
        cout << keynames[ctr] << '\n';
        for (const auto notename : scale)
        {
            cout << ' ' << notename << " 16";
        }
        const auto rests{16 - (scale.size() % 16)};
        for (auto r{0}; r < rests; ++r)
        {
            cout << " r 16";
        }
        cout << '\n';
    }
    cout << "END_OF_TRACK\n";
}
