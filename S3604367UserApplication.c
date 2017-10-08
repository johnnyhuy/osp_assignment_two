/*
 * @author Johnny Huynh (not original author)
 * @date   03 October 2017
 * @version 0.1
 * @brief  A Linux user space program that communicates with the S3604367Device.c LKM. It passes a
 * string to the LKM and reads the response from the LKM. For this example to work the device
 * must be called /dev/S3604367Device.
 * Source code from Derek Molloy at http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
*/

#include "S3604367UserApplication.h"

// Change new line to blank after stdin
char *trim_string(char *string) { 
   size_t length = strlen(string) - 1;

   if (string[length] == '\n') {
      string[length] = '\0';
   }

   return string;
}

int main() {
   int ret, fd;
   char stringToSend[BUFFER_LENGTH];

   printf("Starting device test code example...\n");

   // Open device for r/w access
   fd = open("/dev/S3604367Device", O_RDWR);

   // Check if device can be opened
   if (fd < 0) {
      perror("Failed to open the device...");
      return errno;
   }

   // Prompt to send string to LKM and get string
   printf("Type in a short string to send to the kernel module:\n");
   fgets(stringToSend, sizeof(stringToSend), stdin);

   // Write message to LKM
   printf("Writing message to the device [%s].\n", stringToSend);
   ret = write(fd, trim_string(stringToSend), strlen(stringToSend));

   // Error check 
   if (ret < 0) {
      perror("Failed to write the message to the device.");
      return errno;
   }

   // Confirm to read back and begin read
   printf("Press ENTER to read back from the device...\n");
   getchar();
   printf("Reading from the device...\n");
   ret = read(fd, receive, BUFFER_LENGTH);
   
   if (ret < 0) {
      perror("Failed to read the message from the device.");
      return errno;
   }

   printf("The received message is: [%s]\n", receive);
   printf("End of the program\n");

   return 0;
}
