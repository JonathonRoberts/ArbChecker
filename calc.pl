#!/usr/bin/perl

use strict;
use warnings;
use Switch;

sub calculate{
	my @odds;
	if($ARGV[0]){
		foreach(@ARGV){
			unless(/^\d+\.?\d*$/){die "Invalid Input!\n"}
		}
		@odds = &removecomission(@ARGV);
	}
	else{@odds = (2,3);}
	my $returnodds = &setreturn(@odds);
	my $totalstake = 100;
	my $firstrunflag = 0;

	do{
		if($firstrunflag == 0){
			system("clear");
			$firstrunflag++;
		}
		else{
			chomp;
			switch ($_){
				case "c" {@odds = &changeodds; $returnodds = &setreturn(@odds);}
				case "q" {return;}
				case /^\d+\.?\d*$/ {system("clear;");$totalstake = $_;}
				else {system("clear;");print "Invalid input!\n";last;}
			}
		}

		#Output
		print "c - change odds : q - quit\n";
		printf("Total stake: %.2f\n",$totalstake);
		my $returncash = $totalstake * $returnodds;
		printf("Total return: %.2f\n",$returncash);
		print "Odds\tStake\n";
		foreach(@odds){
			printf("%.2f\t%.2f\n",$_,$returncash/$_);
		}
		print "Input total stake: ";
	}while(<STDIN>)
}
sub removecomission{
	my @odds = @_;
	foreach(0..$#odds){
		if($odds[$_] =~ s/-(.+)//){
			$odds[$_]--;
			$odds[$_] *= 1-($1/100);
			$odds[$_]++;
		}
	}
	return @odds;
}

sub changeodds{
	print "To add a comission, place it after the outcome seperated by \"-\"\,\ne.g: 2.75-2 2.8 3 \n";
	print "Enter odds seperated by a space: ";
	while(<>){
		chomp;
		if(/\b0+\b/){
			print "Enter odds seperated by a space: ";
			next;
		}
		s/^\s+//;
		if(/^(\d+\.?\d*(-\d*\.?\d*)?\s+)(\d\.?\d*(-\d*\.?\d*)?\s*)*$/){
			$_ =~ s/\s+/ /;
			system("clear;");
			print "Odds changed!\n";
			return &removecomission(split(/ /,$_));
		}
		print "Enter odds seperated by a space: ";
	}
}



sub setreturn{
	#This has to be repeated otherwise the loop only recognises the first element in @_
	my $returnodds = 0;
	foreach(@_){
		$returnodds += (1/$_);
	}
	$returnodds = 0;
	foreach(@_){
		$returnodds += (1/$_);
	}
	return 1/$returnodds;

}

&calculate;
