#!/usr/bin/perl

use strict;
use warnings;

#Prints the resultant odds after a single non runner and rule 4 has been applied
# !Note!
# The actual returns will be lower as we are only using the best odds

sub getodds{
	#Gets odds from the comparison page, returns a list of "data-best-dig" values
	my $url = $_[0];
	my $html = qx{curl --insecure --silent $url};
	my @html = split /\n/,$html;
	my @odds;

#	#Filter out live matches
#	#unless($live){
#		foreach(@html){
#			if(/class=\"button no-arrow blink in-play\"\>In Play\</){return -100;}
#	#}
#}

	#Find best odds
	foreach(@html){
		while(/data-best-dig/){
			s/data-best-dig\=\"(\d+\.?\d*)\"/$1/;
			push(@odds,$1);
		}
	}
	if(length($odds[0])){return @odds;}
	else { return -100;}
}

sub getgames{
	#searches a list of games for winner markets
	my $url = $_[0];
	my $html = qx{curl --insecure --silent $url};
	my @html = split /\n/,$html;
	my @urls;
	my $tmp;
	foreach(@html){
		while(m#href\=\"[^\s]+/winner\"#){
			s#href\=\"([^\s]+/winner)\"##;
			$tmp = $1;
			if(!($tmp=~/^https/)){
				$tmp = "/" . $tmp if(!($tmp =~ m#^/#));
				$tmp = "https://www.oddschecker.com$tmp";
			}
			else {next;}

			push(@urls, $tmp);
		}
	}
	return @urls;
}

sub rule4{
	my @odds; #Odds after being input
	my $deduction;#The amount we reduce odds by when applying rule4
	my @table;#This is the table with all the reduced odds
	my $n =0;#blanks
	my $c = 0;#columns
	my $r = 0;#rows
	my @stakes;#total we stake when dutching
	my $percentagereturn = 0;#Percent return on a dutched bet
	my $nonrunnerstake;#temporary value holding the stake for the non runner
	my $high=0;#high water mark for percentage return
	my $low=999;#low water mark for percentage return
	my $avg=0;#average percentage return
	my $counter=0;#counter for average return
	my $tmpreturn=0;#return value that will at some point store every possible percentage return

	#Remove dud odds and place them into array @odds, also calculates percentage return
	foreach(@_){
		unless($_ == 9999){
			push(@odds,$_);
			$percentagereturn += 1/$_;
		}

	}

	#protection against division by 0
	if ($percentagereturn == 0|| $#odds == 0){
		return -100;
	}
	#calculate percentage return
	$percentagereturn = 100/$percentagereturn;

	#calculate the required stake for dutching and place into array
	foreach(@odds){
		push(@stakes, $percentagereturn / $_ );
	}

	#calculate rule4 deductions foreach outcome
	foreach my $dropout (@odds){

		if    ($dropout<=1.11)	{$deduction = 1-.90;}
		elsif ($dropout<1.20)	{$deduction = 1-.85;}
		elsif ($dropout<1.28)	{$deduction = 1-.80;}
		elsif ($dropout<1.34)	{$deduction = 1-.75;}
		elsif ($dropout<1.45)	{$deduction = 1-.70;}
		elsif ($dropout<1.58)	{$deduction = 1-.65;}
		elsif ($dropout<1.67)	{$deduction = 1-.60;}
		elsif ($dropout<1.84)	{$deduction = 1-.55;}
		elsif ($dropout<2.00)	{$deduction = 1-.50;}
		elsif ($dropout<2.25)	{$deduction = 1-.45;}
		elsif ($dropout<2.60)	{$deduction = 1-.40;}
		elsif ($dropout<2.80)	{$deduction = 1-.35;}
		elsif ($dropout<3.40)	{$deduction = 1-.30;}
		elsif ($dropout<4.20)	{$deduction = 1-.25;}
		elsif ($dropout<5.50)	{$deduction = 1-.20;}
		elsif ($dropout<7.00)	{$deduction = 1-.15;}
		elsif ($dropout<11.0)	{$deduction = 1-.10;}
		elsif ($dropout<=15)	{$deduction = 1-.05;}
		else			{$deduction = 1;}


		#Apply deduction to odds and place into @table
		foreach(@odds){
			if($c == $n){$table[$r][$c++] = "X";}
			else{ $table[$c++][$r] =(1+(($_-1)*$deduction));}
		}

		$c = 0;
		$n++;
		$r++;
	}

	#Prints tables showing all possible odds
	#print " " x 6 . "      " x($#odds/2) . "non runner\n";
	#print " odds|";
	#foreach (@odds){
	#	printf("%5s ",$_);
	#}
	#print "\n";
	##print table
	#foreach $r(0..$#odds){
	#	printf("%5s ",$odds[$r]);
	#	foreach $c(0..$#odds){
	#		printf("%5s ",$table[$r][$c]) ;
	#	}
	#	print "\n";
	#}
	#print "\n";

	foreach $c(0..$#odds){
		#Find the X and use this to find the stake of the non runner
		foreach $r(0..$#odds){
			if($table[$r][$c] eq "X" || $table[$r][$c] eq ""){
				$nonrunnerstake =  $stakes[$r] ;
			}
		}
		#Cycle through the rows and calculate the percentage return for each possible outcome by
		#multiplying the original stake with the new odds and adding the stake of the non runner
		foreach $r(0..$#odds){
			unless($table[$r][$c] eq "X" || $table[$r][$c] eq ""){
				$tmpreturn = $stakes[$r] * $table[$r][$c] + $nonrunnerstake ;
				$high = $tmpreturn if($tmpreturn > $high);
				$low = $tmpreturn if($tmpreturn < $low);
				$avg += $tmpreturn;
				$counter++;
			}

		}
	}

	#Calculate values and print output
	$avg /=$counter;
	#print"normal = $returnodds\nhigh = $high\nlow = $low\navg = $avg\n"
	my $worst = $low - $percentagereturn;
	my $mean = $avg - $percentagereturn;
	my $best = $high - $percentagereturn;
	printf("\n %6.3f  | %6.3f | %6.3f || ",$worst,$mean,$best);
	printf("%7.3f | %7.3f | %7.3f | %7.3f\t",$low,$avg, $percentagereturn,$high);
	return $mean;


}

sub crawlrule4{
	my $input = "https://www.oddschecker.com/horse-racing";
	print("\nCrawling winner markets from $input\nand calculating the change inprofit when rule 4 is applied to a dutched bet\n\n");
	print "    Difference in return   ||\t\t Actual return\t\t  |\tRace\n";
	print "Worstcase|  Mean  |Bestcase||Worstcase|  Mean   |  Normal |Bestcase\n";
	foreach(&getgames($input)){
		if(&rule4(&getodds($_)) ==-100){
			print ".";
			next;
		}
		else{print "$_ ";}
	}
}
&crawlrule4;
