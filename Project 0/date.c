#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "date.h"

struct date {
	int year, month, day;
};

Date *date_create(char *datestr) {
	// 
	Date *date = malloc(sizeof(Date));
	if (date != NULL) {
		char* split = strtok(datestr, "/");
		date->day = atoi(split);
		split = strtok(NULL, "/");
		date->month = atoi(split);
		split = strtok(NULL, "/");
		date->year = atoi(split);
	}
	//printf("%d/%d/%d", date->day, date->month, date->year);
	return date;
}

Date *date_duplicate(Date *d) {
	Date *date_dup = malloc(sizeof(d));
	if (date_dup != NULL) {
		date_dup->day = d->day;
		date_dup->month = d->month;
		date_dup->year = d->year;
	}

	return date_dup;
}

int date_compare(Date *date1, Date *date2){
	if (date1->year > date2->year) {
		return 1;
	}
	else if (date1->year < date2->year) {
		return -1;
	}
	else {
		if (date1->month > date2->month) {
			return 1;
		}
		else if (date1->month < date2->month) {
			return -1;
		}
		else {
			if (date1->day > date2->day) {
				return 1;
			}
			else if (date1->day < date2->day) {
				return -1;
			}
			else {
				return 0;
			}
		}

	}
}

void date_destroy(Date *d) {
	free(d);
	
}
