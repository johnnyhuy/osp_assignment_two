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

/*
 * Trims new line from string
 *
 * @return char string
*/
char *trim_s(char *string) { 
   size_t length = strlen(string) - 1;

   if (string[length] == '\n') {
      string[length] = '\0';
   }

   return string;
}

void get_input(char *string, int length) {
   fgets(string, length, stdin);
   trim_s(string);
}

int main() {
   int ret, fd;
   int exit = 0;
   char *menu = (char *) malloc(2);
   char string_to_send[BUFFER_LENGTH];

   // Open device for r/w access
   printf("Opening S3604367 character device...\n");
   fd = open("/dev/S3604367Device", O_RDWR);

   // Check if device can be opened
   if (fd < 0) {
      perror("Failed to open the device...");
      return errno;
   }
   
   while (!exit) {
      printf("\nCharacter Device Menu");
      printf("\n  (w) Write device");
      printf("\n  (r) Read device");
      // TODO: Empty device operation
      // printf("\n  (e) Empty device");
      printf("\n  (q) Quit program");
      printf("\nEnter your choice: ");
      
      get_input(menu, MIN_MENU_INPUT + 2);
      menu = realloc(menu, (strlen(menu) + 1) * sizeof(char));
      
      if (strcmp(menu, "w") == 0) {
         // Prompt to send string to LKM and get string
         printf("\nType in a short string to send to the kernel module:\n");
         get_input(string_to_send, BUFFER_LENGTH);
       
         // Write message to LKM
         printf("Writing message to the device [%s].\n", trim_s(string_to_send));
         ret = write(fd, trim_s(string_to_send), strlen(string_to_send));

         // Error check 
         if (ret < 0) {
            perror("Failed to write the message to the device.");
            return errno;
         }
      }
      else if (strcmp(menu, "r") == 0) {
         printf("\nReading from the device...\n");
         ret = read(fd, receive, BUFFER_LENGTH);
         
         if (ret < 0) {
            perror("Failed to read the message from the device.");
            return errno;
         }

         printf("The received message is: [%s]\n", receive);
      }
      // TODO
      /*
      else if (strcmp(menu, "e")) {
      }
      */
      else if (strcmp(menu, "q") == 0) {
         exit = 1;
      }
      else {
         fprintf(stderr, "Error: invalid menu input, please try again\n");
      }

      // Reset input by reading the remaining chars in stdin
      if (strlen(menu) > 1) {
         int c;
         fprintf(stderr, "Error: input too long, must be %d character(s).\n", MIN_MENU_INPUT);
         while ((c = fgetc(stdin)) != EOF && c != '\n');
      }
   }

   free(menu);

   printf("\nEnd of the program\n");

   return 0;
}
