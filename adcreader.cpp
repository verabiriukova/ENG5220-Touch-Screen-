#include "adcreader.h"
#include <QDebug>





/* Constructor */

ADCreader::ADCreader()
{
int ret = 0;
static const char *device = "/dev/spidev0.0";
bits = 8;
speed = 40000;
delay = 1000;
mode = SPI_CPHA;
// setup ringbuffer
data = new coordinates[BUFFER_SIZE];
// initially, write pointer points to beginning of buffer
writePt = data;
// so does the read pointer
readPt = data;



	/* Setup SPI */

int no_tty = !isatty( fileno(stdout) );

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");
	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
	if (ret == -1)
		pabort("can't set spi mode");
	ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
	if (ret == -1)
		pabort("can't get spi mode");

	ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't set bits per word");
	ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
	if (ret == -1)
		pabort("can't get bits per word");
	/*
	* max speed hz
	*/
	ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't set max speed hz");
	ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
	if (ret == -1)
		pabort("can't get max speed hz");
	fprintf(stderr, "spi mode: %d\n", mode);
		fprintf(stderr, "bits per word: %d\n", bits);
	fprintf(stderr, "max speed: %d Hz (%d KHz)\n", speed, speed/1000);

	/* GPIO Setup for Interrupt line */
	gpio_export(drdy_GPIO);
	// set to input
	gpio_set_dir(drdy_GPIO,0);
	// set interrupt detection to falling edge
	gpio_set_edge(drdy_GPIO,"rising");
	// get a file descriptor for the GPIO pin
	sysfs_fd = gpio_fd_open(drdy_GPIO);

}


void ADCreader::pabort(const char *s) //error printing
{
	perror(s);
	abort();
}


void ADCreader::readData(int fd)
{
  int ret;
  uint8_t tx1[5] = {0};
  uint8_t rx1[5] = {0};

  int i;
  bool packetOK = true;
  struct coordinates temp;
  struct spi_ioc_transfer tr;
  tr.tx_buf = (unsigned long)tx1;
  tr.rx_buf = (unsigned long)rx1;
  tr.len = ARRAY_SIZE(tx1);
  tr.delay_usecs = delay;
  tr.speed_hz = speed;
  tr.bits_per_word = bits;
  
  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
  if (ret < 1){ 
    pabort("can't send spi message");
	packetOK = false;
}
  else{
  	
	for(i=0; i<5; i++){
	  if((i==0) & ((rx1[0] & 0x80)==0)){ //check bit 7 of byte 0, if set data is corrupt
		 packetOK = false;//mark corrupt packet
		 break; //abort
		}
	  else{ //else, process remaining 4 bytes and merge them into 2 coordinates
	  	switch(i){
		case 1:
		temp.x = rx1[i];
		break;
		case 2:
		temp.x = temp.x | rx1[i] << 7;
		break;
		case 3:
		temp.y = rx1[i];
		break;
		case 4:
		temp.y = temp.y | rx1[i] << 7;
		break;
		default:
		break;
		}
	}	
}
}
	if(packetOK){ bufferWrite(&temp); //if data was successfully received, store in buffer
	//fprintf(stderr,"x = %d, y = %d  \n", temp.x, temp.y);
}
	else fprintf(stderr,"Corrupt Packet \n");


}

void ADCreader::bufferWrite(struct coordinates *adcData){
	 (*writePt).x = (*adcData).x;
	 (*writePt).y = (*adcData).y;
	
	if (writePt == (&data[BUFFER_SIZE-1]))
		writePt = data;
	else
		writePt++;		
}

bool ADCreader::isEmpty(){

	return(writePt==readPt);
}


void ADCreader::bufferRead(struct coordinates *appData){

(*appData).x = (*readPt).x;
(*appData).y = (*readPt).y;
fprintf(stderr,"Reading to buffer x = %d, y = %d \n", (*appData).x, (*appData).y);

	if (readPt == (&data[BUFFER_SIZE-1]))
		readPt = data;
	else
		readPt++;		
}

void ADCreader::run()
{
int ret;
	running = true;
	while (running) {
 	ret = gpio_poll(sysfs_fd,1000);
	     if (ret<1) {
	     fprintf(stderr,"Poll error %d\n",ret);
	    }
	    else {
		readData(fd);
		}

		
	}
}

void ADCreader::quit()
{
	running = false;
	close(fd);
	gpio_fd_close(sysfs_fd);
	exit(0);
}
