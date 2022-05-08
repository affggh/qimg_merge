/*
  Program by affggh
  include/timing.h from "https://github.com/JacobLinCool"
  example :
	char tag = "Start [1/1] ";
	timing_start(tag);
	sleep(3);
	printf("%s: %Lg ms\n")
  use this to count (ms) ...
  
  This program need libxml2-dev to parser rawprogram*.xml
*/

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <strings.h>
#include <limits.h>
#include <libgen.h> 
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "timing.h"

#define AUTHOR "affggh"
#define VERBOSE true
#define VERSION "1.0"

#define EXTMAGIC "\x53\xef"

void showMe() {
	fprintf(stdout, "Author : %s\n" \
					"Version : %s\n" \
					"---->>START MERGE\n" \
					, AUTHOR, VERSION);
}

void Usage() {
	fprintf(stdout, "Program by affggh@coolapk.com\n" \
					"Usage:\n" \
					"    qimg-merge [rawprogram*.xml] [partition name]\n\n" \
					"Example:\n" \
					"    qimg-merge rawprogram_unsparse.xml system\n\n");
}

bool detectExtMagic(const char *infile) {
	FILE *fp = NULL;
	fp = fopen(infile, "rb");
	if( fp == NULL ) {
		fprintf(stderr, "Error : File %s does not exist !", infile);
		return false;
	}
	char buf[2];
	fseek(fp, 1080, SEEK_SET);
	fread(buf, 2, 1, fp);
	if( memcmp(buf, EXTMAGIC, 2) == 0 ) {
		return true;
	} else {
		return false;
	}
	fclose(fp);
}

int main(int argc, char *argv[]) {
	char *p, *xmlfile;
	char xmlrpath[512], *outdir, outfile[512];
	FILE *x;
	xmlDocPtr progxml = NULL;
	xmlNodePtr progcur = NULL;
	
	if (argc < 2 || argc > 3){
		Usage();
		return 0;
	} else if (argc < 3) {
		xmlfile = argv[1];
		p = "system";
	} else if (argc < 4) {
		xmlfile = argv[1];
		p = argv[2];
	}
	showMe();
	
	char *tag = "Merge file tooks";
	timing_start(tag);
	
	fprintf(stdout, "XML file : [%s]\n", xmlfile);
	fprintf(stdout, "Partition : [%s]\n", p);
	outdir = dirname(realpath(xmlfile, xmlrpath));
	fprintf(stdout, "OUTDIR : [%s]\n", outdir);
	strcpy(outfile, outdir);
	strcat(outfile, "/");
	strcat(outfile, p);
	strcat(outfile, ".img");
	fprintf(stdout, "OUTFILE : [%s]\n", outfile);

	x = fopen(xmlfile, "r");
	if (x == NULL){
		fprintf(stderr, "Error : error file %s does not exist", xmlfile);
		return 1;
	}
	progxml = xmlParseFile(xmlfile);
	if(progxml == NULL) {
		fprintf(stderr, "Error : XML file parser failed !");
		return 1;
	}
	progcur = xmlDocGetRootElement(progxml);
	if (progcur == NULL){
		fprintf(stderr, "Error : Empty xml document !");
		xmlFreeDoc(progxml);
		return 1;
	}
	if(xmlStrcmp(progcur->name, BAD_CAST "data")) {
		fprintf(stderr, "Error : Invalid xml document !\nXML Root node is not \"data\"");
		xmlFreeDoc(progxml);
		return 1;
	}
	
	progcur = progcur->xmlChildrenNode;
	printf("Start parser xml file\n");
	
	int process = 0, xprocess = 1;
	xmlChar *start_sector, *lb, *SECTOR_SIZE_IN_BYTES, *filename, *num_partition_sectors;
	
	while (progcur != NULL) {
		if ((!xmlStrcmp(progcur->name, (const xmlChar *)"program"))) {
			lb = xmlGetProp(progcur, (const xmlChar *)"label");
			if((!xmlStrcmp(lb, (const xmlChar *)p))) {
				process++;
			}
			xmlFree(lb);
		}
		progcur = progcur->next;
	}
	
	progcur = xmlDocGetRootElement(progxml);
	progcur = progcur->xmlChildrenNode;
	//printf("process : [%d]", process);
	int realoff = 0, doff = 0;
	int i;
	char filenamep[512];
	char *buffer;
	FILE *fp1, *fp2;
	if((fp1 = fopen(outfile, "wb"))==NULL){
		fprintf(stderr, "Error : File create failed !");
		return 1;
	};
	while (progcur != NULL) {
		if ((!xmlStrcmp(progcur->name, (const xmlChar *)"program"))) {
			lb = xmlGetProp(progcur, (const xmlChar *)"label");
			if((!xmlStrcmp(lb, (const xmlChar *)p))) {
				filename = xmlGetProp(progcur, (const xmlChar *)"filename");
				strcpy(filenamep, outdir);
				strcat(filenamep, "/");
				strcat(filenamep, (const char *)filename);
				fopen(filenamep, "rb");
				start_sector = xmlGetProp(progcur, (const xmlChar *)"start_sector");
				num_partition_sectors = xmlGetProp(progcur, (const xmlChar *)"num_partition_sectors");
				SECTOR_SIZE_IN_BYTES = xmlGetProp(progcur, (const xmlChar *)"SECTOR_SIZE_IN_BYTES");
				if(doff == 0) {
					doff = atoi((const char *)start_sector) * atoi((const char *)SECTOR_SIZE_IN_BYTES);
					if(!detectExtMagic((const char *)filenamep)){
						fprintf(stderr, "Warning : Image format may not ext !\n");
					}
					printf("doff = %d\n", doff);
				}
				realoff = atoi((const char *)start_sector) * atoi((const char *)SECTOR_SIZE_IN_BYTES) - doff;
				printf("realoff = %d\n", realoff);
				if(VERBOSE==true){
					printf("start_sector: %s\n", start_sector);
					printf("SECTOR_SIZE_IN_BYTES : %s\n", SECTOR_SIZE_IN_BYTES);
					printf("filename : %s\n", filename);
					printf("filepath : %s\n", filenamep);
				}
				fseek(fp1, realoff, SEEK_SET);
				fprintf(stdout, "Merging at [%s] [%d/%d]\n", filenamep, xprocess, process);
				if((fp2 = fopen(filenamep, "rb"))==NULL){
					fprintf(stderr, "Error : file %s does not exist !", filenamep);
					return 1;
				}
				buffer = (char *)malloc(atoi((const char *)SECTOR_SIZE_IN_BYTES));
				for(i=0;i<atoi((const char *)num_partition_sectors);i++){
					fread(buffer, atoi((const char *)SECTOR_SIZE_IN_BYTES), 1, fp2);
					fwrite(buffer, atoi((const char *)SECTOR_SIZE_IN_BYTES), 1, fp1);
				}
				fclose(fp2);
				xmlFree(start_sector);
				xprocess++;
			}
			xmlFree(lb);
		}
		progcur = progcur->next;
	}

	printf("<<----Merge Done\n");
	xmlFreeDoc(progxml);
	fclose(fp1);
	printf("%s: %Lg s\n", tag, timing_check(tag)/1000);
	return 0;
}