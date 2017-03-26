#!/usr/bin/perl
print "Content-Type: text/html\n\n";

use strict;
use warnings;

package check;

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
			s/data-best-dig\=\"(.{1,5})\"/$1/;
			push(@odds,$1);
		}
	}
	return @odds;
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

print "Enter url of matches page: ";
chomp(my $input = <STDIN>);
print "\n";

foreach(&getgames($input)){
	my $profit = &profit(&getodds($_));
	if($profit > 0){
		printf("\n%s \n%.3f%%",$_,$profit);
	}
	else{print ".";}

}
