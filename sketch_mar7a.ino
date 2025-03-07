#include <RF24.h>
#include <SPI.h>
#include <ezButton.h>
#include "esp_bt.h"
#include "esp_wifi.h"

SPIClass *spiVspi;
SPIClass *spiHspi;

RF24 vspiRadio(22, 21, 16000000);
RF24 hspiRadio(16, 15, 16000000);

unsigned int vspiFlag = 0;
unsigned int hspiFlag = 0;
int vspiChannel = 45;
int hspiChannel = 45;


void channelHopping(int minChannel, int maxChannel) {
    
    vspiChannel += (vspiFlag == 0) ? 4 : -4;

 
    hspiChannel += (hspiFlag == 0) ? 2 : -2;

    if ((vspiChannel > maxChannel && vspiFlag == 0) || (vspiChannel < minChannel && vspiFlag == 1)) {
        vspiFlag = !vspiFlag; 
    }
    if ((hspiChannel > maxChannel && hspiFlag == 0) || (hspiChannel < minChannel && hspiFlag == 1)) {
        hspiFlag = !hspiFlag; 
    }

    // Aktualizacja kanałów na nadajnikach
    vspiRadio.setChannel(vspiChannel);
    hspiRadio.setChannel(hspiChannel);
}


void randomizeChannels() {
    vspiRadio.setChannel(random(80));
    hspiRadio.setChannel(random(80));
    delayMicroseconds(random(60));
}

void setup() {
    Serial.begin(9600);
    esp_bt_controller_deinit();
    esp_wifi_stop();
    esp_wifi_deinit();
    esp_wifi_disconnect();
    initializeVspi();
    initializeHspi();
}

void initializeVspi() {
    spiVspi = new SPIClass(VSPI);
    spiVspi->begin();
    if (vspiRadio.begin(spiVspi)) {
        Serial.println("VSPI Initialized");
        vspiRadio.setAutoAck(false);
        vspiRadio.stopListening();
        vspiRadio.setRetries(0, 0);
        vspiRadio.setPALevel(RF24_PA_MAX, true);
        vspiRadio.setDataRate(RF24_2MBPS);
        vspiRadio.setCRCLength(RF24_CRC_DISABLED);
        vspiRadio.startConstCarrier(RF24_PA_MAX, vspiChannel);
    } else {
        Serial.println("VSPI Initialization Failed");
    }
}

void initializeHspi() {
    spiHspi = new SPIClass(HSPI);
    spiHspi->begin();
    if (hspiRadio.begin(spiHspi)) {
        Serial.println("HSPI Initialized");
        hspiRadio.setAutoAck(false);
        hspiRadio.stopListening();
        hspiRadio.setRetries(0, 0);
        hspiRadio.setPALevel(RF24_PA_MAX, true);
        hspiRadio.setDataRate(RF24_2MBPS);
        hspiRadio.setCRCLength(RF24_CRC_DISABLED);
        hspiRadio.startConstCarrier(RF24_PA_MAX, hspiChannel);
    } else {
        Serial.println("HSPI Init Failed");
    }
}

void loop() {

    channelHopping(1,79);
}
