#!/usr/bin/perl

use strict;
use warnings;

#Prints the resultant odds after a single dropout and applying rule 4

sub getodds{
	#Gets odds from the comparison page, returns a list of "data-best-dig" values
	my $url = $_[0];
	my $html = qx{curl --insecure --silent $url};
	my @html = split /\n/,$html;
	my @odds;

	#Filter out live matches
	#unless($live){
		foreach(@html){
			if(/class=\"button no-arrow blink in-play\"\>In Play\</){return -100;}
	#}
}

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
	my @odds;
	my $deduction;
	my @table;
	my $n =0;#blanks
	my $c = 0;#columns
	my $r = 0;#rows
	my $percentageprofit = 0;
	my @stakes;
	my $returnodds = 0;

	foreach(@_){
		unless($_ == 9999){
			push(@odds,$_);
			$returnodds += 1/$_;
		}

	}

	unless ($returnodds == 0){
		$returnodds = 100/$returnodds;
	}

	foreach(@odds){
		push(@stakes, $returnodds / $_ );
	}

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


		#Apply deduction to odds and print
		foreach(@odds){
			if($c == $n){$table[$r][$c++] = "X";}
			else{ $table[$c++][$r] =(1+(($_-1)*$deduction));}
		}

		$c = 0;
		$n++;
		$r++;
	}

	my $nonrunnerstake;
	my $high=0;
	my $low=999;
	my $avg=0;
	my $counter=0;
	my $tmpreturn=0;
	foreach $c(0..$#odds){
		foreach $r(0..$#odds){
			if($table[$r][$c] eq "X" || $table[$r][$c] eq ""){
				$nonrunnerstake =  $stakes[$r] ;
			}
		}
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
	unless($counter == 0){
		$avg /=$counter;
		#print"normal = $returnodds\nhigh = $high\nlow = $low\navg = $avg\n"
		my $worst = $low - $returnodds;
		my $mean = $avg - $returnodds;
		my $best = $high - $returnodds;
		#	printf("Worstcase %6.3f | Mean %6.3f Bestcase : %6.3f || ",$worst,$mean,$best);
		#	printf(printf("Worstcase %7.3f | Mean %7.3f | Normal %7.3f | Bestcase : %7.3f\t",$low,$avg, $returnodds,$high);
			printf(" %6.3f  | %6.3f | %6.3f || ",$worst,$mean,$best);
			printf("%7.3f | %7.3f | %7.3f | %7.3f\t",$low,$avg, $returnodds,$high);
	}

}

sub crawlrule4{
	my $input = "https://www.oddschecker.com/horse-racing";
	print("Crawling winner markets from $input\nand calculating the change inprofit when rule 4 is applied to a dutched bet\n\n");
	print "     Difference in odds    ||\t\t  Actual odds\t\t  |\tRace\n";
	print "Worstcase|  Mean  |Bestcase||Worstcase|  Mean   |  Normal |Bestcase\n";
	foreach(&getgames($input)){
		&rule4(&getodds($_));
		print "$_\n";
		}


}


print "\n";
&crawlrule4;
