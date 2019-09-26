#ifndef CORE_OUTPUT_H
#define CORE_OUTPUT_H 1

#define SECTION 0 // st_type: section
#define TITLE   1 // st_type: title
#define ITEM    2 // st_type: item
#define	ERROR	3 // st_type: error

#define STD     0 // standard format
#define XML     1 // xml format
#define GRP     2 // grepable format

int OUTPUT_FORMAT;

extern int OutputHandler(int st_format,int st_type,char *fmt,char *st_data);
extern int WriteStd(int st_type,char *st_fmt,char *st_data);
extern int WriteXml(int st_type,char *st_fmt,char *st_data);
extern int WriteGrp(int st_type,char *st_fmt,char *st_data);

#endif