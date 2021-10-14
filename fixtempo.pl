#!/usr/bin/env perl
use strict;
use warnings;
use POSIX;
#  tom janzen 2020-07-21
#  fixtempo.pl.  FIx the tempo in a textmidi text file.

my $arglen = @ARGV;
if (2 ne $arglen)
{
	print $0, " textmidifile factor ; textmidifile.fixed.txt is created\n";
	exit 0;
}

my $filein = $ARGV[0];
my $factor = $ARGV[1];
my $fileout = $filein;
$fileout =~ s/txt/fixed.txt/g;

open my $in,  '<', $filein  or die " no input  file\n";
open my $out, '>', $fileout or die " no output file\n";

print $out "\n";

while ( <$in> )
{
    chomp;
    if (/DELAY ([[:digit:]]+) /)
    {
        my $delay_value = $1;
        $delay_value *= $factor;
        $delay_value = floor($delay_value + .5);
        s/DELAY ([[:digit:]]+)/DELAY $delay_value/;
    }
    print $out $_ . "\n";
}
