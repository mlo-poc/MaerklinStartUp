#include <Railuino.h>

const word    LOCO  = 2;
const word    TIME  = 2000;
const word    IR_PAUSE = 300;

const word F_WARN = 1;
const word F_DIESEL = 2;
const word F_HORN = 3;
const word F_LIGHT = 0;

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

  delay(2 * TIME);

  // Erst mal hupen
  tci.toggleLocoFunction(LOCO, F_HORN);
  Serial.println("Hupen");
  delay(IR_PAUSE);
 
//Erst mal hupen
  tci.toggleLocoFunction(LOCO, F_LIGHT);
  Serial.println("licht an");
  delay(IR_PAUSE);
  
  // Dann anfahren
  Serial.print("Anfahren ");
  for (int n = 0; n < 12; n++) { // n++ ist eine Abkürzung für n=n+1
    tci.accelerateLoco(LOCO);
    Serial.print(n);
    Serial.print(", ");
    // Sicherheitshalber eine kurze Pause zwischen den Beschleunigungs-Kommandos
    delay(IR_PAUSE);
  }

  // Das war's
  // Bis zum reset bleib ich in der Endlosschleife
  for (;;);
}
