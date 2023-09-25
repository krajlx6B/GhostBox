#include "main.h"

void main(void){
	int display;		//0: freq (default), 1: volume, 2: delay
	unsigned char ghostMode;
	unsigned char delayms;
	unsigned char counter;

	display = 0;
	ghostMode = 0;
	delayms = 250;
	counter = 0;

	UartInit();
	ledInit();
	radioInit();

	while(1){   
		counter ++;

		if(counter == 255) {
			counter = 0;
			display = 0;
		}

		//Display
		switch (display) {
		case 0:
			displayFreq();
			break;
		case 1:
			displayVol();
			break;
		case 2:
			displayDelay(delayms);
			break;
		default:
			break;
		}
	
		//Key control
		if(SW_VOL_UP == 0) {
			setVolumeUp();			
			delay_ms(100);
			//PrintSerial("Volume %i\n", getVolume());
			if(ghostMode == 0) display = 1;
			counter = 0;
		}
		if(SW_VOL_DOWN == 0) {
			setVolumeDown();
			delay_ms(100);
			//PrintSerial("Volume %i\n", getVolume());
			if(ghostMode == 0) display = 1;
			counter = 0;
		}
		if(SW_SEEK_UP == 0 && ghostMode == 0) {
			seekUp();
			delay_ms(1000);
			//PrintSerial("Station %i\n", getFreq());
			display = 0;
		}
		/*if(SW_SEEK_DOWN == 0) {
			seekDown();
			delay_ms(1000);
			PrintSerial("Station %i\n", getFreq());
		}*/
		if(SW_SEEK_DOWN == 0) {
			ghostMode = 1;
			delay_ms(100);
		}
			
		if(ghostMode == 1) {	
			display = 0;
			ghostSeek();
			delay_ms(delayms);

			if(SW_SEEK_UP == 0) {
				if(delayms == 350) delayms = 50;
				else delayms = delayms + 50;
				//PrintSerial("Delay %i\n", delayms);
				//display = 2;
				delay_ms(50);
			}
			if(SW_SEEK_DOWN == 0) {
				ghostMode = 0;
				break;
			}	
		}		
	}
}