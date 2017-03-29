#!/usr/bin/perl
print "Content-Type: text/html\n\n";

# This program will search for arbitrage opportunities from oddschecker
# from a list of matches such as: https://www.oddschecker.com/football
#
# TODO:
# get list of markets to work out all arbs for a game, use the list from:
# https://www.oddschecker.com/american-football/nfl/super-bowl-lii/betting-markets

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
	my $url = "https://www.oddschecker.com/sitemap.xml";
	my $html = qx{curl --insecure --silent $url};
	my @pages;
	while($html =~ s#(https://www\.oddschecker\.com/[^\<]+)/sitemap\.xml##){
		push(@pages,$1)
	}
	return @pages;
}
sub uniq {
	    my %seen;
	        grep !$seen{$_}++, @_;
	}

sub list{
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
#&list;


print("Input events filter(e.g. football): ");
chomp(my $input = <STDIN>);
foreach my $workingpage(&crawlsite){
	#Filter
	if($workingpage =~ m#$input#){

		foreach(&getgames($workingpage)){
			my $profit = &profit(&getodds($_));
			if($profit > 0.5){
				printf("\n%s \n%.3f%%",$_,$profit);
			}
			else{print ".";}
		}
	}
}
print "\n";

#print "Enter url of matches page: ";
#chomp(my $input = <STDIN>);
#print "\n";
#
#foreach(&getgames($input)){
#	my $profit = &profit(&getodds($_));
#	if($profit > 0){
#		printf("\n%s \n%.3f%%",$_,$profit);
#	}
#	else{print ".";}
#
#}
#
#print "\n";
