/*
 * XMHF rich guest app for hyperdep hypapp
 * author: amit vasudevan (amitvasudevan@acm.org)
 */

#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>

typedef unsigned char u8;
typedef unsigned int u32;
typedef unsigned long long int u64;


#define PAGE_SHIFT 12
#define PAGEMAP_LENGTH 8


//////
// hyperdep test harness

//////////////////////////////////////////////////////////////////////////////
// xhhyperdep test

__attribute__((aligned(4096))) static u8 testxhhyperdep_page[4096];

#define HYPERDEP_ACTIVATEDEP			0xC0
#define HYPERDEP_DEACTIVATEDEP			0xC1

typedef void (*DEPFN)(void);

static void __vmcall(u32 eax, u32 ebx, u32 edx){
	asm volatile (
			"movl %0, %%eax \r\n"
			"movl %1, %%ebx \r\n"
			"movl %2, %%edx \r\n"
			"vmcall \r\n"
			: /*no output*/
			: "g" (eax), "g" (edx), "g" (ebx)
			: "%eax", "%ebx", "%edx"
	);
}


static u64 va_to_pa(void *vaddr) {
	FILE *pagemap;
	unsigned long offset;
	u64 page_frame_number = 0;

	// open the pagemap file for the current process
	pagemap = fopen("/proc/self/pagemap", "rb");
	if(pagemap == NULL){
		printf("\n%s: unable to open pagemap file. exiting!", __FUNCTION__);
		exit(1);
	}

	// seek to the page that vaddr is on
   offset = (unsigned long)vaddr / getpagesize() * PAGEMAP_LENGTH;
   if(fseek(pagemap, (unsigned long)offset, SEEK_SET) != 0) {
      printf("\n%s: Failed to seek pagemap to proper location", __FUNCTION__);
      exit(1);
   }

   // The page frame number is in bits 0-54 so read the first 7 bytes and clear the 55th bit
   fread(&page_frame_number, 1, PAGEMAP_LENGTH-1, pagemap);

   page_frame_number &= 0x7FFFFFFFFFFFFF;

   fclose(pagemap);

   return (page_frame_number << PAGE_SHIFT);
}



void do_testxhhyperdep(void){
    u32 gpa = &testxhhyperdep_page;
    DEPFN fn = (DEPFN)gpa;

    testxhhyperdep_page[0] = 0xC3; //ret instruction

    printf("\n%s: Going to activate DEP on page %x", __FUNCTION__, gpa);

    __vmcall(HYPERDEP_ACTIVATEDEP,  0, gpa);

    printf("\n%s: Activated DEP", __FUNCTION__);

    //fn();

    printf("\n%s: Going to de-activate DEP on page %x", __FUNCTION__, gpa);

    __vmcall(HYPERDEP_DEACTIVATEDEP,  0, gpa);

    printf("\n%s: Deactivated DEP", __FUNCTION__);

}



void main(void){
    printf("\n%s: DEP buffer at 0x%08x", __FUNCTION__, &testxhhyperdep_page);

    printf("\n%s: proceeding to lock DEP page...", __FUNCTION__);

    //lock the DEP page in memory so we have it pinned down
	if(mlock(&testxhhyperdep_page, sizeof(testxhhyperdep_page)) == -1) {
		  printf("\nFailed to lock page in memory: %s\n", strerror(errno));
		  exit(1);
	}

    printf("\n%s: DEP page locked", __FUNCTION__);

    printf("\n%s: DEP buffer at paddr=%08x", __FUNCTION__, va_to_pa(&testxhhyperdep_page));

	do_testxhhyperdep();

    printf("\n%s: proceeding to unlock DEP page...", __FUNCTION__);

    //unlock the DEP page
	if(munlock(&testxhhyperdep_page, sizeof(testxhhyperdep_page)) == -1) {
		  printf("\nFailed to unlock page in memory: %s\n", strerror(errno));
		  exit(1);
	}

    printf("\n%s: DEP page unlocked", __FUNCTION__);


    printf("\n\n");
}
