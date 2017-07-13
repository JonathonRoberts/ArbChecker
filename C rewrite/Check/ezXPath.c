#include <stdio.h>
#include <libxml/xpath.h>
#include <libxml/HTMLparser.h>
#include "ezcurl.c"

 /*
 * Simple functions for running Xpath queries on XML and HTML
 * return value is the number of results found
 */

int extern ezXPathXML(char *website,char *expr,char *output[]);
int extern ezXPathXMLFile(char *file,char *expr,char *output[]);
int extern ezXPathHTML(char *website,char *expr,char *output[]);
int extern ezXPathHTMLFile(char *file,char *expr,char *output[]);

xmlNodeSetPtr getnodeset(xmlDocPtr doc,xmlChar *xpath);

int extern ezXPathXML(char *website,char *expr,char *output[]){
	xmlInitParser();
	int i;
	int size = 0;
	xmlDocPtr doc = xmlReadDoc(ezcurl(website),website,NULL, 1);
	xmlChar *xpath = (xmlChar*) expr;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlChar *element;
	result = getnodeset(doc,xpath);

	if(result){
		nodeset = result->nodesetval;
		size = nodeset->nodeNr;
		*output=malloc(size);
		for(i=0;i<size;i++){
			if(strlen(xmlNodeListGetString(doc,nodeset->nodeTab[i]->xmlChildrenNode,1)) == 0){
				output[i] = (char *)malloc(4);
				strcpy(output[i] , "");
			}
			else{
				element = xmlNodeListGetString(doc,nodeset->nodeTab[i]->xmlChildrenNode,1);
				output[i] = (char *)malloc(strlen(element));
				strxfrm(output[i],element,strlen(element)+1);
				xmlFree(element);
			}
		}
		xmlXPathFreeObject(result);
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return size;
}

int extern ezXPathXMLFile(char *file,char *expr,char *output[]){
	xmlInitParser();
	int i;
	int size = 0;
	 xmlDocPtr doc = xmlReadFile(file, NULL, 1);
	xmlChar *xpath = (xmlChar*) expr;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlChar *element;
	result = getnodeset(doc,xpath);

	if(result){
		nodeset = result->nodesetval;
		size = nodeset->nodeNr;
		*output=malloc(size);
		for(i=0;i<size;i++){
			if(strlen(xmlNodeListGetString(doc,nodeset->nodeTab[i]->xmlChildrenNode,1)) == 0){
				output[i] = (char *)malloc(4);
				strcpy(output[i] , "");
			}
			else{
				element = xmlNodeListGetString(doc,nodeset->nodeTab[i]->xmlChildrenNode,1);
				output[i] = (char *)malloc(strlen(element));
				strxfrm(output[i],element,strlen(element)+1);
				xmlFree(element);
			}
		}
		xmlXPathFreeObject(result);
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return size;
}
int extern ezXPathHTML(char *website,char *expr,char *output[]){
	xmlInitParser();
	int i;
	int size = 0;
	xmlDocPtr doc = htmlReadDoc(ezcurl(website),website,NULL, HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET | HTML_PARSE_IGNORE_ENC );
	xmlChar *xpath = (xmlChar*) expr;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlChar *element;
	result = getnodeset(doc,xpath);

	if(result){
		nodeset = result->nodesetval;
		size = nodeset->nodeNr;
		*output=malloc(size);
		for(i=0;i<size;i++){
			if(strlen(xmlNodeListGetString(doc,nodeset->nodeTab[i]->xmlChildrenNode,1)) == 0){
				output[i] = (char *)malloc(4);
				strcpy(output[i] , "");
			}
			else{
				element = xmlNodeListGetString(doc,nodeset->nodeTab[i]->xmlChildrenNode,1);
				output[i] = (char *)malloc(strlen(element));
				strxfrm(output[i],element,strlen(element)+1);
				xmlFree(element);
			}
		}
		xmlXPathFreeObject(result);
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return size;
}

int extern ezXPathHTMLFile(char *file,char *expr,char *output[]){
	xmlInitParser();
	int i;
	int size = 0;
	 xmlDocPtr doc = htmlReadFile(file, NULL, HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING | HTML_PARSE_NONET);
	xmlChar *xpath = (xmlChar*) expr;
	xmlNodeSetPtr nodeset;
	xmlXPathObjectPtr result;
	xmlChar *element;
	result = getnodeset(doc,xpath);

	if(result){
		nodeset = result->nodesetval;
		size = nodeset->nodeNr;
		*output=malloc(size);
		for(i=0;i<size;i++){
			if(strlen(xmlNodeListGetString(doc,nodeset->nodeTab[i]->xmlChildrenNode,1)) == 0){
				output[i] = (char *)malloc(4);
				strcpy(output[i] , "");
			}
			else{
				element = xmlNodeListGetString(doc,nodeset->nodeTab[i]->xmlChildrenNode,1);
				output[i] = (char *)malloc(strlen(element));
				strxfrm(output[i],element,strlen(element)+1);
				xmlFree(element);
			}
		}
		xmlXPathFreeObject(result);
	}

	xmlFreeDoc(doc);
	xmlCleanupParser();
	return size;
}
xmlNodeSetPtr getnodeset(xmlDocPtr doc,xmlChar *xpath){
	xmlXPathObjectPtr result;
	xmlXPathContextPtr context;
	context = xmlXPathNewContext(doc);
	if(context==NULL){
		/*printf("Error in xmlXpathNewContext\n");*/
		return NULL;
	}
	result = xmlXPathEvalExpression(xpath,context);
	xmlXPathFreeContext(context);
	if(result==NULL){
		/*printf("Error in xmlXpathEvalExpression\n");*/
		return NULL;
	}
	if(xmlXPathNodeSetIsEmpty(result->nodesetval)){
		xmlXPathFreeObject(result);
		/*printf("No Results\n");*/
		return NULL;
	}
	return result;
}
