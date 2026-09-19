#include <regex.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(){
	int fd = open("b.txt",O_RDWR);
	int size = lseek(fd,0,SEEK_END);//返回文件大小
	char * mmap_ptr = NULL;
	mmap_ptr = mmap(NULL,size,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0);
	close(fd);

	regex_t reg;
	const char*regStr="<a[^>]*href=\"([^\"]+)\"[^>]*>([^<]+)</a>";
	int ret=regcomp(&reg, regStr, REG_EXTENDED);
	if(ret!=0){
		regfree(&reg);
		munmap(mmap_ptr,size);
		return -1;
	}
	
	int matchNum=3;
	regmatch_t match[matchNum];

	char* start_ptr=mmap_ptr;
	while(1){
		int res=regexec(&reg,start_ptr,matchNum,match,0);

		if(res!=0){
			break;
		}

		char buf[1024];
		int len=match[1].rm_eo-match[1].rm_so;
		strncpy(buf,start_ptr + match[1].rm_so,len);
		buf[len]='\0';

		printf("链接:%s\n",buf);

		len=match[2].rm_eo-match[2].rm_so;
		strncpy(buf,start_ptr + match[2].rm_so,len);
		buf[len]='\0';

		printf("标题:%s\n",buf);

		
		start_ptr=start_ptr+match[0].rm_eo;
	}

	regfree(&reg);
	munmap(mmap_ptr,size);

	return 0;
}

