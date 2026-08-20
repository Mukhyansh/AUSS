// gcc -O1 compress.c -o compress
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include "trie.c"

#define MAX_SEQUENCE_SIZE 65536

static inline void write_code_to_file(FILE *file,uint32_t code,size_t bytes_per_code){
	fwrite(&code,bytes_per_code,1,file);
}

int main(void){
	FILE *input_file=fopen("1.txt","r");
	if(!input_file){
		printf("Failed opening input file.\n");
		return 1;
	}

	FILE *output_file=fopen("compressed.dat","wb");
	if(!output_file){
		printf("Failed opening output file.\n");
		return 1;
	}

	fseek(output_file,sizeof(long int),SEEK_SET);
	fseek(output_file,sizeof(long int),SEEK_CUR);
	fseek(output_file,sizeof(uint32_t),SEEK_CUR);

	unsigned char c=fgetc(input_file);
	if(feof(input_file)){
		printf("File is empty.\n");
		return 1;
	}

	long int file_offset_24bit_codes=0;
	long int file_offset_32bit_codes=0;

	trie *tree=maketrie();
	uint32_t current_code=0;

	for(current_code=0;current_code<256;current_code++)
		insertchar(tree,(unsigned char)current_code,current_code);

	trie *p;
	bool char_found=searchchar(tree,c,&p);
	assert(char_found);

	size_t bytes_per_code=2;

	while(!feof(input_file)){
		c=fgetc(input_file);

		trie *p_plus_c;
		if(searchchar(p,c,&p_plus_c)){
			p=p_plus_c;
		}
		else{
			insertchar(p,c,current_code++);
			write_code_to_file(output_file,p->value,bytes_per_code);

			char_found=searchchar(tree,c,&p);
			assert(char_found);

			if(current_code==0xFFFF){
				bytes_per_code=3;
				file_offset_24bit_codes=ftell(output_file);
			}
			else if(current_code==0xFFFFFF){
				bytes_per_code=4;
				file_offset_32bit_codes=ftell(output_file);
			}
		}
	}

	write_code_to_file(output_file,p->value,bytes_per_code);

	fseek(output_file,0,SEEK_SET);
	fwrite(&file_offset_24bit_codes,sizeof(long int),1,output_file);
	fwrite(&file_offset_32bit_codes,sizeof(long int),1,output_file);
	fwrite(&current_code,sizeof(uint32_t),1,output_file);

	fclose(output_file);
	fclose(input_file);
	freenode(tree);

	return 0;
}
