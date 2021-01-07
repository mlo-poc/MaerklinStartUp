#include <Railuino.h>

const word    LOCO  = 2;
const word    TIME  = 2000;

TrackControllerInfrared tci = TrackControllerInfrared();

// === TrackControllerInfrared =======================================
//TrackControllerInfrared::TrackControllerInfrared() {
//boolean TrackControllerInfrared::sendMessage(word address, word command) {
//boolean TrackControllerInfrared::accelerateLoco(int loco) {
//boolean TrackControllerInfrared::decelerateLoco(int loco) {
//boolean TrackControllerInfrared::toggleLocoDirection(int loco) {
//boolean TrackControllerInfrared::toggleLocoFunction(int loco, int function) {
//boolean TrackControllerInfrared::setPower(boolean power) {
//boolean TrackControllerInfrared::setTurnout(int turnout, boolean through) {
//void TrackControllerInfrared::start() {
//boolean TrackControllerInfrared::getPower() {

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  tci.start();
  
  Serial.print("Power Status: ");
  Serial.println(tci.getPower());

  delay(TIME);

  tci.setPower(true);
  Serial.print("Power Status: ");
  Serial.println(tci.getPower());
}


void loop() {

  delay(2*TIME);

//  for (int n = 0; n < 5; n++) {
//    Serial.print(n);
//    Serial.print(": accelerateLoco(");
//    Serial.print(LOCO);
//    Serial.println(")");
//    tci.accelerateLoco(LOCO);
//    delay(TIME);
//  };
//  for (int n = 0; n < 12; n++) {
//    Serial.print(n);
//    Serial.print(": decelerateLoco(");
//    Serial.print(LOCO);
//    Serial.println(")");
//    tci.decelerateLoco(LOCO);
//    delay(TIME);
//  };
//  for (;;);
}
