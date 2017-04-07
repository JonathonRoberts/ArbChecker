#!/usr/bin/perl

# This program will search for arbitrage opportunities from oddschecker
# from a list of matches such as: https://www.oddschecker.com/football

use strict;
use warnings;

sub profit{
	my $percentageprofit = 0;
	foreach(@_){
		$percentageprofit += (100/$_);
	}
	return 100-$percentageprofit;
}

sub getodds{
	#Gets odds from the comparison page, returns a list of "data-best-dig" values
	my $url = $_[0];
	my $html = qx{curl --insecure --silent $url};
	my @html = split /\n/,$html;
	my @odds;
	foreach(@html){
		while(/data-best-dig/){
			s/data-best-dig\=\"(\d+\.?\d*)\"/$1/;
			push(@odds,$1);
		}
	}
	if(length($odds[0])){return @odds;}
	else { return 1;}
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

sub crawlsite{
	#searches the sitemap for all markets, returns list of all markets
	my $url = "https://www.oddschecker.com/sitemap.xml";
	my $html = qx{curl --insecure --silent $url};
	my @pages;
	while($html =~ s#(https://www\.oddschecker\.com/[^\<]+)/sitemap\.xml##){
		push(@pages,$1)
	}
	return @pages;
}
sub uniq {
	#Quick function to remove duplicates from an array
	    my %seen;
	        grep !$seen{$_}++, @_;
	}

sub list{
	#returns list of catagories found in the sitemap
	my @oldlist;
	foreach(&crawlsite){
		$_ =~ s#^.+\.com/([^\/]+).*#$1#;
		push(@oldlist,$_);
	}
	my @newlist = &uniq(@oldlist);
	foreach(@newlist){
		print "$_\n";
	}
	return @newlist;
}

sub getallmarkets{
	#given the winners market this subroutine searches the betting-markets page and returns a list of all the markets for the event
	my $url = $_[0];
	$url=~ s/winner\/?$/betting-markets\//;
	my $markets = qx{curl --insecure --silent $url};
	my $filterurl;
	my @markets;
	if($url =~ m#^https\://www\.oddschecker\.com(.+)/betting-markets/#){
		$filterurl = $1;
	}
	else{die "invalid url $_[0]\n"};
	while($markets =~ s/\"$filterurl([^\"]+)//){
			push(@markets,$_[0] . $1);
	}
	return @markets;
}

sub findall{
	#Using a simple search returns all markets for search results
	print("Searching all markets\n");
	print("Input events filter(e.g. football): ");
	chomp(my $input = <STDIN>);
	foreach my $page(&crawlsite){
			#Filter
			if($page =~ m#$input#){
				foreach my $markets (&getgames($page)){
					foreach my $workingpage (&getallmarkets($markets)){
						my $profit = &profit(&getodds($workingpage));
						if(-0.1 < $profit && $profit < 15){
							printf("\n%s \n%.3f%%",$workingpage,$profit);
						}
						else{print ".";}
				}
			}
		}
	}
	print "\n";
}

sub findwinners{
	#Using a simple search returns all winner markets for search results
	print("Searching winner markets\n");
	print("Input events filter(e.g. football): ");
	chomp(my $input = <STDIN>);
	foreach my $workingpage(&crawlsite){
		#Filter
		if($workingpage =~ m#$input#){

			foreach(&getgames($workingpage)){
				my $profit = &profit(&getodds($_));
				if(-0.1 < $profit && $profit < 15){
					printf("\n%s \n%.3f%%",$_,$profit);
				}
				else{print ".";}
			}
		}
	}
	print "\n";
}

sub findallfrompage{
	#Given a page e.g https://www.oddschecker.org/football returns all markets found from the page
	print("Searching all markets\n");
	print "Enter url of matches page: ";
	chomp(my $input = <STDIN>);
	foreach(&getallmarkets(&getgames($input))){
		my $profit = &profit(&getodds($_));
		if(-0.1 < $profit && $profit < 15){
			printf("\n%s \n%.3f%%",$_,$profit);
		}
		else{print ".";}
	}
	print "\n";
}

sub findwinnersfrompage{
	#Given a page e.g https://www.oddschecker.org/football returns all winner markets found from the page
	print("Searching winner markets\n");
	print "Enter url of matches page: ";
	chomp(my $input = <STDIN>);
	foreach(&getgames($input)){
		my $profit = &profit(&getodds($_));
		if(-0.1 < $profit && $profit < 15){
			printf("\n%s \n%.3f%%",$_,$profit);
		}
		else{print ".";}

	}
	print "\n";
}

#&findall;
&findwinners;
#&findallfrompage;
#&findwinnersfrompage;
#&list;
