#!/usr/bin/perl
print "Content-Type: text/html\n\n";
use strict;
use warnings;

#Provides statistics extracted from .csv of matched/arbitrage betting logs
#All commas and GBP signs must be removed from spreadsheet

my $date;
my $bookie;
my $net;
my $NET;
my @values;
my %bookieprofit;
my %dateprofit;
my @sortedoutput;

open FILE, $ARGV[0] or die "File not found!\n";
while(<FILE>){
	@values = (split(",", $_));
	if($values[0] eq "Total profit"){next;}
	unless($values[2] eq ""||$values[11] eq ""||$values[10] eq ""){
		$date = $values[2];
		$date =~ s/\///g;
		$bookie = $values[10];
		$net = $values[11];
		$net =~ s/\?//;
		$bookieprofit{lc($bookie)} += $net;
		$dateprofit{$date} += $net;
		$NET +=$net;
	}
}

for my $key (keys %bookieprofit){
	my $tmp = sprintf"%-12s %+.2f",$key, $bookieprofit{$key};
	push(@sortedoutput,$tmp);
}
@sortedoutput = sort (@sortedoutput);

foreach(@sortedoutput){
	print "$_\n";
}
print "\nNet profit = $NET\n\n";
exit;
