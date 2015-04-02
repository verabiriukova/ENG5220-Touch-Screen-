#ifndef ADCREADER
#define ADCREADER

#include <QThread>


#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
// gpio library used for interrupt line
#include "gpio-sysfs.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#define BUFFER_SIZE 255


/* Data Structures */
	struct coordinates{
  	uint16_t x;
  	uint16_t y;
	} ;


class ADCreader : public QThread
{
public:

	

	/* Methods */
	ADCreader(); //constructor
	void quit();
	void run();
	void bufferRead(struct coordinates *appData);
	bool isEmpty();
	
private:

	/* Methods */

	void readData(int fd); 
	void bufferWrite(struct coordinates *adcData);
	void pabort(const char *s);
	/* variables */
	bool running;

	/* CONSTANTS */
//	static const char *device = "/dev/spidev0.0";
	uint8_t mode;
	uint8_t bits;
	uint32_t speed;
	uint16_t delay;
	static const unsigned int drdy_GPIO = 14; 
	
	
	/* Ring Buffer */
	coordinates *data; //pointer to start of ring buffer
	coordinates *writePt; //write pointer
	coordinates *readPt;  //read pointer

	/* File Descriptors */
	int fd;
	int sysfs_fd;

};

#endif
