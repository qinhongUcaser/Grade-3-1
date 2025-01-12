#include <assert.h>

#include <elf.h>

#include <errno.h>

#include <stdarg.h>

#include <stdio.h>

#include <stdlib.h>

#include <string.h>



void write_bootblock(FILE *image, FILE *bbfile, Elf32_Phdr *Phdr);

Elf32_Phdr *read_exec_file(FILE *opfile);

uint8_t count_kernel_sectors(Elf32_Phdr *Phdr);

void extent_opt(Elf32_Phdr *Phdr_bb, Elf32_Phdr *Phdr_k, int kernelsz);


/*
Elf32_Phdr *read_exec_file(FILE *opfile)

{	

	char ehdr[4];

	Elf32_Phdr *phdr;

	

	fseek(opfile,28,1);



	fread(&ehdr,4,1,opfile);

	//printf("test%s\n",ehdr);

	phdr = (Elf32_Phdr*)ehdr;

	//printf("%x\n",(unsigned int)phdr);

}
*/


uint8_t count_kernel_sectors(Elf32_Phdr *Phdr)

{

	uint8_t num;

	long int temp;

	temp = (*Phdr).p_memsz%512;

	

	if(temp == 0)

		return num = (*Phdr).p_memsz/512;

	else

		return num = (*Phdr).p_memsz/512 + 1;

}


/*
void write_bootblock(FILE *image, FILE *file, Elf32_Phdr *phdr)

{

	char offset[4];

	FILE* stream;

	int a;

	//printf("%x\n",(unsigned int)file);

	

	fseek(file,4,1);

	stream = file;

	//printf("%d\n",a);

	//printf("%x\n",(unsigned int)file);

	a = fread(&offset,4,1,stream);

	printf("%d\n",a);

	printf("%s\n",offset);

	fwrite(file,(*phdr).p_filesz,1,image);

}
*/

/*
void write_kernel(FILE *image, FILE *knfile, Elf32_Phdr *Phdr, int kernelsz)

{

	knfile = (FILE*)(*Phdr).p_paddr;

	fwrite(knfile,(*Phdr).p_filesz,1,image);

}
*/


void record_kernel_sectors(FILE *image, uint8_t kernelsz)

{

	fwrite(&kernelsz,sizeof(kernelsz),1,image);

}



void extent_opt(Elf32_Phdr *Phdr_bb, Elf32_Phdr *Phdr_k, int kernelsz)

{

	printf("kernel_size:%dB\n",sizeof(char)*512*(kernelsz+1));

	printf("bb_location:%d,size:%dB\n",0,(*Phdr_bb).p_filesz);

	printf("k_location:1~%d,sizeof:%dB\n",kernelsz,(*Phdr_k).p_filesz);

}



int main()

{

	uint8_t kernelsz;

	FILE *bootblock;

	FILE *kernel;

	FILE *image;

	Elf32_Phdr *Phdr_bb;
    Elf32_Phdr Phdr_bb_1;
	Elf32_Phdr *Phdr_k;
    Elf32_Phdr Phdr_k_1;
    uint8_t phoff[4];
    uint8_t code[1024];
	bootblock = fopen("/home/stu/Desktop/project-2/start_code/bootblock","rb");
	kernel = fopen("/home/stu/Desktop/project-2/start_code/kernel","rb");
	image = fopen("/home/stu/Desktop/project-2/start_code/image","wb");

    //定位seg header
	fseek(bootblock,28,0);
	fread(phoff,4,1,bootblock);
    //存储seg header
    fseek(bootblock,phoff[0],0);
    fread(&Phdr_bb_1,32,1,bootblock);
    Phdr_bb = &Phdr_bb_1;
    //找到segment
    fseek(bootblock,Phdr_bb_1.p_offset,0);
    fread(code,Phdr_bb_1.p_filesz,1,bootblock);
    //写入bootblock
    fwrite(code,Phdr_bb_1.p_filesz+1,1,image);

    //定位seg header
    fseek(kernel,28,0);
    fread(phoff,4,1,kernel);
    //存储seg header
    fseek(kernel,phoff[0],0);
    fread(&Phdr_k_1,32,1,kernel);
    Phdr_k = &Phdr_k_1;
    //zhaodaosegment
    fseek(kernel,Phdr_k_1.p_offset,0);
    fread(code,Phdr_k_1.p_filesz,1,kernel);


    kernelsz = count_kernel_sectors(Phdr_k);

    //确定写入位置并写入
    fseek(image,Phdr_bb_1.p_filesz+1,0);
    record_kernel_sectors(image,kernelsz);

	fclose(image);
	image = fopen("/home/stu/Desktop/project-2/start_code/image","ab");  


    //写入kernel
    fwrite(code,Phdr_k_1.p_filesz+1,1,image);
	

	extent_opt(Phdr_bb,Phdr_k,kernelsz);

	

	fclose(image);

	fclose(bootblock);

	fclose(kernel);

}
