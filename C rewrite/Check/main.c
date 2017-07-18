#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <unistd.h>
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
	char sport;
	char *title;
	int date;
	int noodds;
	int live;
	float returnodds;
};

float setreturn(int noods, float odds[]);
int scanwinner(char * website);
int footballquick();
int findraces();
int crawlall(char *search);

struct Market Markets[400];
int arrno = 0;

void printstruct(int i);

void getoptlonghelp(char *progname){
	fprintf(stderr,"\
Usage: %s [-adhlqu |-s <string>|-u <url>]\n\
--all, -a\t\t\t- search all markets, default search is just winner markets\n\
--display, -d\t\t\t- display the highest level of searchable markets\n\
--help, -h\t\t\t- show this help message\n\
--live, -l\t\t\t- include live matches in search\n\
--quick -q\t\t- quickly searches popular football winner markets\n\
--search <string>, -s <string>\t- search for market which include <string>\n\
--url <url>, -u <url>\t\t- search for markets from a markets page such as https://www.oddschecker.com/football/\n",progname);
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
-u <url>\t- search for markets from a markets page such as https://www.oddschecker.com/football/\n",progname);
}

int liveflag = 0;
int quickflag = 0;
int all = 0;
int crawlallflag = 0;

int main(int argc, char *argv[]){
	if(argc == 1){
		help(argv[0]);
		return 1;
	}
	int ch;
	while ((ch = getopt(argc, argv, "adhlqsu?")) != -1)
			switch(ch) {
			case 'a':
				/*print all markets*/
				all = 1;
				/*FALL THROUGH*/
			case 'd':
				/*prints highest level of markets for searching*/
				break;
			case 'h':
				help(argv[0]);
				break;
			case 'l':
				/*include live matches in search*/
				liveflag = 1;
				/*FALL THROUGH*/
			case 'q':
				/*quick search*/
				quickflag = 1;
				break;
			case 's':
				/*search*/
				crawlallflag = 1;
				break;
			case 'u':
				/*scan from markets url, e.g https://www.oddschecker.com/football/ */
				break;
			case '?':
				help(argv[0]);
				break;
			default:
				help(argv[0]);
	}
	if(quickflag){
		footballquick();
		/*tennisquick();*/
	}
	if(crawlallflag)
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
	int i;
	int size;
	char *xpath = "id('t1')/tr/@data-best-dig |\
		id('t1')/tr/@data-bname";

	/*	XPath for checking date and whether the event is live
		id('betting-odds')/section[1]/div/div/div/div/span[1]|\
		id('betting-odds')/section[1]/div/div/div/div/span[2]/@class|\
	*/

        size = ezXPathHTML(website,xpath,output);
	if(size!=0){

		strlcpy(Markets[arrno].website, website,120);
		for(i =0;i<size;i++){
                        if(i%2==0){
                           Markets[arrno].noodds++;
                           if(i==0){
                              Markets[arrno].odds[i] = atof(output[i]);
			      if(Markets[arrno].odds[i] == 9999.0){
				   Markets[arrno].noodds--;
				   i++;
			      }

                           }
                           else{
                              Markets[arrno].odds[i/2] = atof(output[i]);
			      if(Markets[arrno].odds[i/2] == 9999.0){
				   Markets[arrno].noodds--;
				   i++;
			      }
                           }
                        }
                        else{
                              strlcpy(Markets[arrno].outcome[((i+1)/2)-1] , output[i], 50);

                           }
			free(output[i]);
		}
		if(Markets[arrno].noodds==1){//if only one possible outcome
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
	if(liveflag>=Markets[i].live){
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
		printf("Returnodds = %f\n",Markets[i].returnodds);
		/*
	   printf("sport - %c\n",Markets[i].sport);
	   printf("date - %d\n",Markets[i].date);
	   */
	}
}

float setreturn(int noods, float odds[]){
	int i;
	float returnodds = 0;
	for(i = 0;i<noods;i++){
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
									if(Markets[arrno].returnodds>1){
										printf("\n");
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
