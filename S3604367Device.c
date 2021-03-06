/*
 * @author Johnny Huynh (not original author)
 * @date   03 October 2017
 * @version 0.1
 * @see http://www.derekmolloy.ie/ for a full description and follow-up descriptions.
 * Source code from Derek Molloy at http://derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
 */

#include "S3604367Device.h"

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init char_driver_init(void){
    printk(KERN_INFO "S3604367 Char Device: Initializing the S3604367 Char Device LKM\n");

    // Try to dynamically allocate a major number for the device -- more difficult but worth it
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

    if (majorNumber < 0) {
        printk(KERN_ALERT "S3604367 Char Device failed to register a major number\n");
        return majorNumber;
    }

    printk(KERN_INFO "S3604367 Char Device: registered correctly with major number %d\n", majorNumber);

    // Register device class
    driver_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(driver_class)){                // Check for error and clean up if there is
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class\n");
        return PTR_ERR(driver_class);          // Correct way to return an error on a pointer
    }
    printk(KERN_INFO "S3604367 Char Device: device class registered correctly\n");

    // Create device
    driver_device = device_create(driver_class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

    if (IS_ERR(driver_device)) {               // Clean up if there is an error
        class_destroy(driver_class);           // Repeated code but the alternative is goto statements
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(driver_device);
    }

    printk(KERN_INFO "S3604367 Char Device: device class created correctly\n"); // Made it! device was initialized

    return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit char_driver_exit(void){
    device_destroy(driver_class, MKDEV(majorNumber, 0));     // remove the device
    class_unregister(driver_class);                          // unregister the device class
    class_destroy(driver_class);                             // remove the device class
    unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
    printk(KERN_INFO "S3604367 Char Device: Goodbye from the LKM!\n");
}

/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep){
    numberOpens++;
    printk(KERN_INFO "S3604367 Char Device: Device has been opened %d time(s)\n", numberOpens);
    return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, myDevice.data, size_of_message);

   if (error_count == 0) {            // if true then have success
      printk(KERN_INFO "S3604367 Char Device: Sent %d characters to the user\n", size_of_message);
      return (size_of_message = 0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "S3604367 Char Device: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
}

/** @brief This function is called whenever the device is being written to from user space i.e.
 *  data is sent to the device from the user. The data is copied to the message[] array in this
 *  LKM using the sprintf() function along with the length of the string.
 *  @param filep A pointer to a file object
 *  @param buffer The buffer to that contains the string to write to the device
 *  @param len The length of the array of data that is being passed in the const char buffer
 *  @param offset The offset if required
 */
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
   strcat(myDevice.data, buffer);
   sprintf(myDevice.data, "%s", myDevice.data);   // appending received string with its length
   size_of_message = strlen(myDevice.data); 
   printk(KERN_INFO "S3604367 Char Device: Received %zu characters from the user\n", len);
   return len;
}

/** @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep){
   printk(KERN_INFO "S3604367 Char Device: Device successfully closed\n");
   return 0;
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(char_driver_init);
module_exit(char_driver_exit);
