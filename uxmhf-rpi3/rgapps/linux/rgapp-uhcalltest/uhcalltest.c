/*
 * hypercall test program (uhcalltest)
 * author: amit vasudevan (amitvasudevan@acm.org)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#include <uhcall.h>
#include <uhcalltest.h>


int main(){
   int ret, fd;
   uhcallkmod_param_t uhcallp;

   printf("Starting usr mode hypercall test...\n");

   // Open the device with read/write access
   fd = open("/dev/uhcallkmod", O_RDWR);
   if (fd < 0){
      perror("Failed to open /dev/uhcallkmod...");
      return errno;
   }

   uhcallp.uhcall_function=UAPP_UHCALLTEST_FUNCTION_TEST;
   uhcallp.uhcall_buffer=NULL;
   uhcallp.uhcall_buffer_len=0;

   ret = write(fd, &uhcallp, sizeof(uhcallp));
   if (ret < 0){
      perror("Failed to write the message to the device.");
      return errno;
   }

   printf("End of test\n");
   return 0;
}
