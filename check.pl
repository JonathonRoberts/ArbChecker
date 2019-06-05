#!/usr/bin/perl

# This program will search for arbitrage opportunities from oddschecker
# from a list of matches such as: https://www.oddschecker.com/football

use strict;
use warnings;
use Getopt::Long;


my $allmarketsflag=0;
my $displayflag=0;
my $helpflag=0;
my $live=0;
my $urlflag;
my $search;

GetOptions (
	"all|a" => \$allmarketsflag,
	"display|d" => \$displayflag,
	"help|h" => \$helpflag,
	"live|l" => \$live,
	"search|s=s" => \$search,
	"url|u=s" => \$urlflag,)
or die("Error in command line arguments\n");

if($helpflag){
	print(
"
 --all, -a\t\t\t- search all markets, default search is just winner markets
 --display, -d\t\t\t- display the highest level of searchable markets
 --help, -h\t\t\t- show this help message
 --live, -l\t\t\t- include live matches in search
 --search <string>, -s <string>\t- search for market which include <string>
 --url <url>, -u <url>\t\t- search for markets from a markets page such as https://www.oddschecker.com/football/
\n");
	exit 1;
}

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

	#Filter out live matches
	unless($live){
		foreach(@html){
			if(/class=\"button no-arrow blink in-play\"\>In Play\</){return -100;}
		}
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

sub crawlsite{
	#searches the sitemap for all markets, returns list of all markets
	my $url = "https://www.oddschecker.com/sitemap.xml";
	my $html = qx{curl --insecure --silent $url};
	my @pages;
	while($html =~ s#(https://www\.oddschecker\.com/sport/[^\<]+)/sitemap\.xml##){
my $tmp = $1;
	$tmp =~ s#sport/##;
		push(@pages,$tmp)
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
		$_ =~ s#^.+\.com/sport/([^\/]+).*#$1#;
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
	my $input;
	if($search){
		$input = $search;
		print("Searching all markets for \"$input\"\n");
	}
	else{
		print("Searching all markets\n");
		print("Input events search filter(e.g. football): ");
		chomp($input = <STDIN>);
	}
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
	my $input;
	if($search){
		$input = $search;
		print("Searching all markets for \"$input\"\n");
	}
	else{
		print("Searching winner markets\n");
		print("Input events search filter(e.g. football): ");
		chomp($input = <STDIN>);
	}
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
	#print "Enter url of matches page: ";
	#chomp(my $input = <STDIN>);
	my $input = $_[0];
	unless($input =~ m#^https\://www\.oddschecker\.com/.+#i){
		print "\nInvalid input!\nInput must be of the form: https://www.oddschecker.com/football\n\n";
		exit;
	}
	print("Searching all markets in $input\n");
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
	#print("Searching winner markets\n");
	#print "Enter url of matches page: ";
	#chomp(my $input = <STDIN>);
	my $input = $_[0];
	unless($input =~ m#^https\://www\.oddschecker\.com/.+#i){
		print "\nInvalid input!\nInput must be of the form: https://www.oddschecker.com/football/\n\n";
		exit;
	}
	print("Searching winner markets in $input\n");
	foreach(&getgames($input)){
		my $profit = &profit(&getodds($_));
		if(-0.1 < $profit && $profit < 15){
			printf("\n%s \n%.3f%%",$_,$profit);
		}
		else{print ".";}

	}
	print "\n";
}


if($displayflag){
	&list;
	exit;
}

if($allmarketsflag){
	if($urlflag){
		&findallfrompage($urlflag)
	}
	else{
		&findall;
	}
}
else{
	if($urlflag){
		&findwinnersfrompage($urlflag);
	}
	else{
		&findwinners;
	}
}
