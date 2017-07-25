#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
#include <time.h>
#include "ezXPath.c"

#define MAXELEMENTS 3000 /* Maximum number of results to return */

 /*
 * Using XPath to return the best odds for an event
 */

struct Market{
	char website[120];
	float odds[MAXELEMENTS/2];
	char outcome[MAXELEMENTS/2][50];
	char *bestbookie[MAXELEMENTS/2];
	char sport[50];
	char *title;
	time_t date;
	int noodds;
	int live;
	float returnodds;
};
struct Market Markets[MAXELEMENTS];

float setreturn(int noods, float odds[]);
int scanwinner(char * website);
int footballquick();
int findraces();
int crawlall(char *search);
time_t getdatetime(char *datestring);
static int cmp (const void *p1, const void *p2);
void list();
void printstruct(int i);
void extracturldata(char *website);

void getoptlonghelp(char *progname){
	fprintf(stderr,"\
Usage: %s [-adhlqu |-s <string>|-u <url>]\n\
--all, -a\t\t\t- search all markets, default search is just winner markets\n\
--display, -d\t\t\t- display the highest level of searchable markets\n\
--help, -h\t\t\t- show this help message\n\
--live, -l\t\t\t- include live matches in search\n\
--quick -q\t\t- quickly searches popular football winner markets\n\
--search <string>, -s <string>\t- search for market which include <string>\n",progname);
}
void help(char *progname){
	fprintf(stderr,"\
Usage: %s [-adhlqu |-s <string>|-u <url>]\n\
-a\t\t- search all markets, default search is just winner markets\n\
-d\t\t- display the highest level of searchable markets\n\
-h\t\t- show this help message\n\
-l\t\t- include live matches in search\n\
-q\t\t- quickly searches popular football winner markets\n\
-s <string>\t- search for market which include <string>\n\
-t \t\t- only show events which occur wihtin 24 hours\n",progname);
}

int arrno = 0;
time_t timenow;
time_t timefilter;

int liveflag = 0;
int quickflag = 0;
int all = 0;
int crawlallflag = 0;
int displayflag = 0;
int searchflag = 0;
int timeflag = 0;
int month,day,year;

int main(int argc, char *argv[]){
	if(argc == 1){
		help(argv[0]);
		return 1;
	}
	int ch;
	while ((ch = getopt(argc, argv, "adhlqst?")) != -1)
			switch(ch) {
			case 'a':
				/*print all markets*/
				all = 1;
				break;
			case 'd':
				/*prints highest level of markets for searching*/
				displayflag = 1;
				break;
			case 'h':
				help(argv[0]);
				break;
			case 'l':
				/*include live matches in search*/
				liveflag = 1;
				break;
			case 'q':
				/*quick search*/
				quickflag = 1;
				break;
			case 's':
				/*search*/
				searchflag = 1;
				break;
			case 't':
				/*time*/
				timeflag = 1;
				break;
			case '?':
				help(argv[0]);
				break;
			default:
				help(argv[0]);
	}

	/* initialise time filter */
	time(&timenow);
	timefilter = timenow+24*60*60;
	struct tm *current = localtime(&timenow);
	month = current->tm_mon;
	day = current->tm_mday;
	year = current->tm_year;

	if(all){
		printf("Searching all markets:\n");
		crawlall(".");
	}
	if(displayflag){
		list();
	}
	if(quickflag){
		footballquick();
		/*tennisquick();*/
	}
	if(searchflag)
		crawlall(argv[argc-1]);

	return 1;

	/*
	footballquick();
	crawlall("football");
	findraces();
	*/
}
int scanwinner(char * website){
	char *output[MAXELEMENTS];
	int size;
	int i=0;
	int n = 0;
	char *xpath = "id('betting-odds')/section[1]/div/div/div/div/span[1]|\
		id('betting-odds')/section[1]/div/div/div/div/span[2]/@class|\
		id('t1')/tr/@data-best-dig |\
		id('t1')/tr/@data-bname";
	regex_t regex;
	int datecheck = regcomp(&regex, ".+ [[:digit:]][[:digit:]]:",REG_EXTENDED);
	regex_t regex2;
	int livecheck = regcomp(&regex2, "button no-arrow blink in-play",REG_EXTENDED);

	/*	XPath for checking date and whether the event is live
	*/

	size = ezXPathHTML(website,xpath,output);
	if(size>1){
		if((datecheck = regexec(&regex, output[i],0,NULL,0))==0){
			Markets[arrno].date = getdatetime(output[i]);
			free(output[i]);
				i++;
		}
		else
			Markets[arrno].date = 0;
		if((livecheck = regexec(&regex2, output[i],0,NULL,0))==0){
			Markets[arrno].live = 1;
			free(output[i]);
				i++;
		}

		/* The two statements above attempt to find the date and whether the event
		 * is currently live, they are looking for output[i] which looks like:
		 * Thursday 20th July / 17:30
		 * button no-arrow blink in-play
		*/

		extracturldata(website);
		strlcpy(Markets[arrno].website, website,120);

		Markets[arrno].noodds = 1;
		Markets[arrno].odds[n] = atof(output[i]);
		free(output[i]);
		i++;
		if(Markets[arrno].odds[n] == 9999.0){
			Markets[arrno].noodds--;
			i++;
		}
		else{
			strlcpy(Markets[arrno].outcome[n] , output[i], 50);
			free(output[i]);
			n++;
			i++;
		}

		for(;i<size;i++){
			Markets[arrno].noodds++;
			Markets[arrno].odds[n] = atof(output[i]);
			free(output[i]);
			i++;
			if(Markets[arrno].odds[n] == 9999.0){
				Markets[arrno].noodds--;
			}
			else{
				strlcpy(Markets[arrno].outcome[n] , output[i], 50);
				n++;
			}
			free(output[i]);
		}
		if(Markets[arrno].noodds<=1){//one or less possible outcomes
			Markets[arrno].noodds--;
			return 0;
		}
		Markets[arrno].returnodds = setreturn(Markets[arrno].noodds,Markets[arrno].odds);
		return 1;
	}else{
		return 0;
	}

	/*I'll have to use something like the below to find out the bookie for each outcome
	size = ezXPathHTML("https://www.oddschecker.com/football/champions-league/rijeka-v-tns/winner","id('t1')/tr/td[position() > 1 and not(position() > 26)]/@data-odig",output);
	if(size!=0){
		printf("Found %d elements:\n",size);
		for(i =0;i<size;i++){
			if(i%25==0){
				printf("\n");
			}
			printf("%s ",output[i]);
			free(output[i]);
		}
		printf("\n");
	}
	*/
}

int footballquick(){
	/* football page quickscrape
	 * stops if an outcome is found without any odds
	 */

	char *output[MAXELEMENTS];
	int i;
	int size;
	char *website = "https://www.oddschecker.com/football";
	char *xpath = "id('fixtures')/div/table/tbody/tr/td/p/span/@data-name |\
		id('fixtures')/div/table/tbody/tr/td/@data-best-odds |\
		id('fixtures')/div/table/tbody/tr/td/a/@href|\
		id('fixtures')/div/table/tbody/tr/td/a/@class";
	size = ezXPathHTML(website,xpath,output);
	int n;
	if(size!=0){
		for(i =0;i<size;){
			n = 0;
			Markets[arrno].noodds = 3;
			Markets[arrno].odds[n] = atof(output[i]);
			free(output[i++]);
			strlcpy(Markets[arrno].outcome[n++] , output[i], 50);
			free(output[i++]);
			Markets[arrno].odds[n] = atof(output[i]);
			free(output[i++]);
			strlcpy(Markets[arrno].outcome[n++] , output[i], 50);
			free(output[i++]);
			Markets[arrno].odds[n] = atof(output[i]);
			free(output[i++]);
			strlcpy(Markets[arrno].outcome[n++] , output[i], 50);
			free(output[i++]);
			if(strcmp("button btn-1-small blink in-play",output[i])==0){
				Markets[arrno].live = 1;
			}
			else{
				Markets[arrno].live = 0;
			}
			free(output[i++]);
			strlcpy(Markets[arrno].website , "https://www.oddschecker.com/", 100);
			strlcat(Markets[arrno].website , output[i], 100);
			free(output[i++]);
			if((Markets[arrno].returnodds = setreturn(Markets[arrno].noodds,Markets[arrno].odds))>1)
				printstruct(arrno);
			arrno++;
		}

	}
	return 1;
}

void printstruct(int i){
	if((timeflag == 0||timeflag&&Markets[i].date>=timenow&&Markets[i].date<=timefilter))
	if(liveflag>=Markets[i].live){
		printf("\n");
		/*printf("%s\n",Markets[i].title);*/
		printf("%s\n",Markets[i].website);
		/*
		for(n = 0;n<Markets[i].noodds;n++){
			printf("%f - ",Markets[i].odds[n]);
			printf("%s",Markets[i].outcome[n]);
			printf("%s",Markets[i].bestbookie[n]);
			printf("\n");
		}
		*/
		if(Markets[i].date != 0){ /* Only print the date if we know it */
			struct tm *tm = localtime(&Markets[i].date);
			char s[64];
			strftime(s, sizeof(s), "%c", tm);
			printf("%s\n", s);
		}
		if(Markets[arrno].live == 1)
			printf("LIVE\n");
		printf("Returnodds = %f\n",Markets[i].returnodds);
		/*
		printf("sport - %c\n",Markets[i].sport);
		*/
	}
}

float setreturn(int noodds, float odds[]){
	int i;
	float returnodds = 0;
	for(i = 0;i<noodds;i++){
		returnodds += (1/odds[i]);
	}
	return (1/returnodds);
}
int findraces(){
	/* grab todays UK racing urls */
	char *output[MAXELEMENTS];
	int i;
	int size;
	char *website = "https://www.oddschecker.com/horse-racing";
	char *xpath = "id('mc')/section[1]/div/div/div/div/div/div/a/@href";

	size = ezXPathHTML(website,xpath,output);
	for(i=0;i<size;i++){
		strcpy(Markets[arrno].website,"https://www.oddschecker.com");
		strcat(Markets[arrno].website,output[i]);
		if(scanwinner(Markets[arrno].website)==0){
			continue;
		}
		else if(Markets[arrno].returnodds > 1){
			printf("\n");
			printstruct(arrno++);
		}
		else{
			printf(".");
			arrno++;
		}
	}
	return 1;
}
int crawlall(char *search){
	char *sitemapoutput[MAXELEMENTS];
	char *tmpoutput[MAXELEMENTS];
	int size;
	int i;
	int c;
	int tmpsize;
	regex_t regex2;
	int errorcheck = regcomp(&regex2, "^https://www.oddschecker.com/[^[:space:]]+$",REG_EXTENDED);
	regex_t regex;
	int toplevelsearch = regcomp(&regex, search,REG_EXTENDED);

	size = ezXPathXML("https://www.oddschecker.com/sitemap.xml","/*[local-name() = 'sitemapindex']/*[local-name() = 'sitemap']/*[local-name() = 'loc']",sitemapoutput);

	if(size!=0){
		for(i =2;i<size;i++){
			if((toplevelsearch = regexec(&regex, sitemapoutput[i],0,NULL,0))==0){/*select which branch in sitemap to scan for best odds*/
				printf("Searching all events under: %s\n",sitemapoutput[i]);

				if((errorcheck = regexec(&regex2, sitemapoutput[i],0,NULL,0))==0){/*protects against naff input*/
					tmpsize = ezXPathXML(sitemapoutput[i],"/*[local-name() = 'urlset']/*[local-name() = 'url']/*[local-name() = 'loc']",tmpoutput);
					if(tmpsize!=0){
						for(c = 0;c<tmpsize;c++){
							if((errorcheck = regexec(&regex2, tmpoutput[c],0,NULL,0))==0){/*protects against naff input*/
								if(scanwinner(tmpoutput[c])){
									if(Markets[arrno].returnodds>=1){
										printstruct(arrno++);
									}
									else{
										arrno++;
									}
								}
							}
							free(tmpoutput[c]);
						}
					}
				}
			}
			free(sitemapoutput[i]);
		}
	}
	return 1;
}

static int cmp (const void *p1, const void *p2){
	/* simple ascii sorting for qsort */
	return strcmp(* (char * const *) p1, * (char * const *) p2);
}
void list(){
	char *sitemapoutput[MAXELEMENTS];
	int size;
	int i;
	char tmp[50];
	regex_t regex2;
	int errorcheck = regcomp(&regex2, "^https://www.oddschecker.com/[^[:space:]]+$",REG_EXTENDED);

	size = ezXPathXML("https://www.oddschecker.com/sitemap.xml","/*[local-name() = 'sitemapindex']/*[local-name() = 'sitemap']/*[local-name() = 'loc']",sitemapoutput);

	if(size!=0){
		qsort(sitemapoutput,size,sizeof(char *),cmp);
		for(i =2;i<size;i++){
			if((errorcheck = regexec(&regex2, sitemapoutput[i],0,NULL,0))==0){/*protects against naff input*/
				sscanf(sitemapoutput[i], "https://www.oddschecker.com/sport/%[^/]/sitemap.xml",tmp);
				printf("%s\n",tmp);
			}
			free(sitemapoutput[i]);
		}
	}
}
time_t getdatetime(char *datestring){
	/* Example: datestring = "Friday 21st July / 16:00";*/
	struct tm tm;
	int mon,dom,hh, mm;
	char *dow;
	char *doms;
	char *mons;
	time_t time_value;

	sscanf(datestring, "%s %d%2s %s / %d:%d",&dow, &dom, &doms, &mons, &hh, &mm);

	tm.tm_year = 2017 - 1900;	/* HARDCODED YEAR, I have not seen any markets
					* from 2018 so I don't actually know how it's
					* displayed on Oddschecker
					*/
	if(strcmp((char*)&mons,"January")==0)mon = 0;
	else if(strcmp((char *)&mons,"February")==0)mon = 1;
	else if(strcmp((char *)&mons,"March")==0)mon = 2;
	else if(strcmp((char *)&mons,"April")==0)mon = 3;
	else if(strcmp((char *)&mons,"May")==0)mon = 4;
	else if(strcmp((char *)&mons,"June")==0)mon = 5;
	else if(strcmp((char *)&mons,"July")==0)mon = 6;
	else if(strcmp((char *)&mons,"August")==0)mon = 7;
	else if(strcmp((char *)&mons,"September")==0)mon = 8;
	else if(strcmp((char *)&mons,"October")==0)mon = 9;
	else if(strcmp((char *)&mons,"November")==0)mon = 10;
	else mon = 11;
	tm.tm_mon = mon;

	tm.tm_mday = dom;
	tm.tm_hour = hh;
	tm.tm_min = mm;
	tm.tm_sec = 0;
	tm.tm_isdst = 1;
	time_value = mktime(&tm);
	return time_value;
}

void extracturldata(char *website){
	int hh,mm;
	char shh[4];
	char smm[4];
	char tmp[30];
	char *sport;
	struct tm tm;
	if((sscanf(website,"https://www.oddschecker.com/horse-racing/%[^/]/%2s:%2s/",tmp,shh,smm))==3){
		mm = atoi(smm);
		hh = atoi(shh);
		tm.tm_year = year;
		tm.tm_mon = month;
		tm.tm_mday = day;
		tm.tm_hour = hh;
		tm.tm_min = mm;
		tm.tm_sec = 0;
		tm.tm_isdst = 1;
		Markets[arrno].date = mktime(&tm);
	}
	if((sscanf(website,"https://www.oddschecker.com/greyhounds/%[^/]/%2s:%2s/",tmp,shh,smm))==3){
		mm = atoi(smm);
		hh = atoi(shh);
		tm.tm_year = year;
		tm.tm_mon = month;
		tm.tm_mday = day;
		tm.tm_hour = hh;
		tm.tm_min = mm;
		tm.tm_sec = 0;
		tm.tm_isdst = 1;
		Markets[arrno].date = mktime(&tm);
	}
	if((sscanf(website,"https://www.oddschecker.com/%[^/]/",sport)==1)){
		strlcpy(Markets[arrno].sport,sport,49);
	}
	return;
}
