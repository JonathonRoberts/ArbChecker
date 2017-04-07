#!/usr/bin/perl
print "Content-Type: text/html\n\n";

use strict;
use warnings;

#Prints a table of the resultant odds after a single dropout and applying rule 4
#TODO
# - Replace odds in table with profit % from original stake
# - Add user friendly odds input

my @odds = (2.37,4.5,5,7);
my $deduction;
my @table;
my $n =0;#blanks
my $c = 0;#columns
my $r = 0;#rows

foreach my $dropout (@odds){

	if    ($dropout<=1.11)	{$deduction = 1-.90;}
	elsif ($dropout<=1.18)	{$deduction = 1-.85;}
	elsif ($dropout<=1.25)	{$deduction = 1-.80;}
	elsif ($dropout<=1.30)	{$deduction = 1-.75;}
	elsif ($dropout<=1.40)	{$deduction = 1-.70;}
	elsif ($dropout<=1.53)	{$deduction = 1-.65;}
	elsif ($dropout<=1.62)	{$deduction = 1-.60;}
	elsif ($dropout<=1.80)	{$deduction = 1-.55;}
	elsif ($dropout<=1.95)	{$deduction = 1-.50;}
	elsif ($dropout<=2.00)	{$deduction = 1-.45;}
	elsif ($dropout<=2.25)	{$deduction = 1-.40;}
	elsif ($dropout<=2.60)	{$deduction = 1-.35;}
	elsif ($dropout<=2.80)	{$deduction = 1-.30;}
	elsif ($dropout<=3.40)	{$deduction = 1-.25;}
	elsif ($dropout<=4.20)	{$deduction = 1-.20;}
	elsif ($dropout<=5.50)	{$deduction = 1-.15;}
	elsif ($dropout<=7.00)	{$deduction = 1-.10;}
	elsif ($dropout<=11.0)	{$deduction = 1-.05;}
	else			{$deduction = 1;}


	#Apply deduction to odds and print
	foreach(@odds){
		if($c == $n){$table[$r][$c++] = "X";}
		else{ $table[$c++][$r] =(1+(($_-1)*$deduction));}
	}

	$c = 0;
	$n++;
	$r++;
}

#Print odds and labels
print " " x 6 . "      " x($#odds/2) . "non runner\n";
print " odds ";
foreach (@odds){
	printf("%5s ",$_);
}
print"\n";

#print table
foreach $r(0..$#odds){
	printf("%5s ",$odds[$r]);
	foreach $c(0..$#odds){
		printf("%5s ",$table[$r][$c]) ;
	}
	print "\n";
}
print "\n";
