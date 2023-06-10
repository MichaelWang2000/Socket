#ifndef _PFILE_H_
#define _PFILE_H_

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

#define STR_LEN_256 256
#define STR_LEN_128 128

typedef struct PFILE_HEAD_S
{
	char filename[STR_LEN_128];
	char filetype[STR_LEN_128];
	int datalen;
	int seqno;
}pfilehead_t;

#ifdef __cplusplus
}
#endif

#endif
