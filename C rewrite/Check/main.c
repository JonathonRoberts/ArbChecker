#include <stdio.h>
#include <string.h>
#include "ezXPath.c"

#define MAXELEMENTS 200 /* Maximum number of results to return */

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
	float returnodds;
};

float setreturn(int noods, float odds[]);
int scanwinner(char * website);
int footballquick();
int findraces();

struct Market Markets[400];
int arrno = 0;

void printstruct(int i);

int main(){
	footballquick();
	findraces();
	return 0;
}
int scanwinner(char * website){
	char *output[MAXELEMENTS];
	int i;
	int size;
	char *xpath = "id('t1')/tr/@data-best-dig |\
		id('t1')/tr/@data-bname";
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
		id('fixtures')/div/table/tbody/tr/td/a/@href";
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
			strlcpy(Markets[arrno].website , "https://www.oddschecker.com/", 100);
			strlcat(Markets[arrno].website , output[i], 100);
			free(output[i++]);
			if((Markets[arrno].returnodds = setreturn(Markets[arrno].noodds,Markets[arrno].odds))>1)
				printstruct(arrno++);
		}

	}
	printf("\n");
	return 1;
}

void printstruct(int i){
   int n;
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
   //printf("sport - %c\n",Markets[i].sport);
   //printf("date - %d\n",Markets[i].date);
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
	/* Causing segfaults when scanning this, today Epsom is 7, might be an ezcurl  issue
	 * char *xpath = "id('mc')/section[1]/div/div/div/div[position()=7]/div/div/a/@href";
	 */

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
