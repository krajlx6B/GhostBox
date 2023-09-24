#include "main.h"

void main(void){
	int ghostMode;
	int delayms;

	ghostMode = 0;
	delayms = 50;

	UartInit();
	ledInit();
	radioInit();

	while(1){   
		//Display
		displayDigit(getFreq(), 0);
		displayDigit(getFreq(), 1);
		displayDigit(getFreq(), 2);
		displayDigit(getFreq(), 3);
	
		//Key control
		if(SW_VOL_UP == 0) {
			setVolumeUp();			
			delay_ms(100);
			PrintSerial("Volume %i\n", getVolume());

		}
		if(SW_VOL_DOWN == 0) {
			setVolumeDown();
			delay_ms(100);
			PrintSerial("Volume %i\n", getVolume());
		}
		if(SW_SEEK_UP == 0) {
			seekUp();
			delay_ms(1000);
			PrintSerial("Station %i\n", getFreq());
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
			ghostSeek();
			delay_ms(delayms);

			if(SW_SEEK_UP == 0) {
				if(delayms == 300) delayms = 50;
				else delayms = delayms + 50;

				PrintSerial("Delay %i\n", delayms);
				delay_ms(100);
			}
			
			if(SW_SEEK_DOWN == 0) {
				ghostMode = 0;
				break;
			}	
		}		
	}
}