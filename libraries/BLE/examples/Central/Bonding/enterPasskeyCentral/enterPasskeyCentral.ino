/* enterPasskeyCentral.ino

   Written by Chiara Ruggeri (chiara@arduino.org)

   This example shows how to enable bonding features on BLECentral module.
   To know all the possible bonding types please refer to the documentation.
  
   Use the complementary example showPasskey.ino in File->Examples->BLE->Peripheral->Bonding
   to test this feature.
  
   This example code is in the public domain.
*/

#include <BLECentralRole.h>

// create central instance
BLECentralRole                   bleCentral                    = BLECentralRole();

// create remote service
BLERemoteService                 dummyRemoteService                  = BLERemoteService("19b10010e8f2537e4f6cd104768a1214");

// create remote characteristics 
BLERemoteCharacteristic          dummyRemoteCharacteristic           = BLERemoteCharacteristic("19b10011e8f2537e4f6cd104768a1214", BLERead | BLEWrite);



void setup() {
  Serial.begin(9600);

  // add service and characteristic
  bleCentral.addRemoteAttribute(dummyRemoteService);
  bleCentral.addRemoteAttribute(dummyRemoteCharacteristic);

  // enable bonding and set the type
  bleCentral.enableBond(LESC_CONFIRM_PASSKEY);
  
  // assign event handlers
  bleCentral.setEventHandler(BLEScanReceived, receiveAdvPck);
  bleCentral.setEventHandler(BLEConnected, bleCentralConnectHandler);
  bleCentral.setEventHandler(BLEDisconnected, bleCentralDisconnectHandler);
  bleCentral.setEventHandler(BLEPasskeyRequested, writePasskey);
  bleCentral.setEventHandler(BLEBonded, bond);
  
  // use BLEMessage event handler to retrieve information about internal BLE status
  bleCentral.setEventHandler(BLEMessage, receiveMessage);
  
  // begin initialization
  bleCentral.begin(); 
  
  Serial.println(F("BLE Bonding example"));
}

void loop() {
  // Do nothing
}

void receiveAdvPck(BLEPeripheralPeer& peer){
  char advertisedName[31];
  byte len;
  // search for a device that advertises "BONDExample" name
  peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_SHORT_LOCAL_NAME, advertisedName, len);
  if(len == 0) // field not found
      peer.getFieldInAdvPck(BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME, advertisedName, len);
  if(len != 0){ // the field was found
  Serial.println(advertisedName);
    if(!strcmp(advertisedName, "BONDExample"))
      // Name found. Connect to the peripheral
      bleCentral.connect(peer);
   } 
}

void bleCentralConnectHandler(BLEPeripheralPeer& peer) {
  // peer connected event handler
  Serial.print("Connected event, peripheral: ");
  Serial.println(peer.address());
}

void bleCentralDisconnectHandler(BLEPeripheralPeer& peer) {
  // peer disconnected event handler
  Serial.print("Disconnected event, peripheral: ");
  Serial.println(peer.address());
}

void writePasskey(BLEPeripheralPeer& peer) {
 // peripheral required passkey event handler
  char passkey[6];
  int i=0;
  int j=0;
  Serial.println("Type the 6 digits code you see on the other device");
  while(i < 6){
    j=Serial.available();
      for(int k=0; k<j; k++){
        passkey[i]=Serial.read();
        i++;
        }
   }
   Serial.print("you typed: ");
   Serial.println(passkey);
   bleCentral.sendPasskey(passkey);
}

void bond(BLEPeripheralPeer& peer) {
  // central bonded event handler
  Serial.println("Bonded");
}

void receiveMessage(int evtCode, int messageCode){
  bleCentral.printBleMessage(evtCode, messageCode);
}