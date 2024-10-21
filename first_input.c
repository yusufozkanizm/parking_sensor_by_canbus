#include <SPI.h>
#include <mcp_can.h>

// MCP2515 CAN Controller
const int CAN_CS_PIN = 10; // Chip Select pin for the CAN controller
const int CAN_INT_PIN = 2; // Interrupt pin for the CAN controller
MCP_CAN CAN(CAN_CS_PIN);

const int TRIG_PIN = 6; // Trigger pin of the ultrasonic sensor
const int ECHO_PIN = 7; // Echo pin of the ultrasonic sensor

void setup() {
  Serial.begin(115200); // Initialize serial communication at a baud rate of 115200

  // Initialize CAN
  if (CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_16MHZ) == CAN_OK) { // Start CAN communication at 500 kbps
    Serial.println("CAN Init Success"); // Print success message if CAN is initialized
  } else {
    Serial.println("CAN Init Fail"); // Print failure message if CAN initialization fails
    while (1); // Stay in a loop if CAN initialization fails
  }
  CAN.setMode(MCP_NORMAL); // Set CAN mode to normal

  // Initialize Ultrasonic Sensor
  pinMode(TRIG_PIN, OUTPUT); // Set trigger pin as output
  pinMode(ECHO_PIN, INPUT); // Set echo pin as input
}

unsigned int measureDistance() {
  // Send a 10us pulse to trigger
  digitalWrite(TRIG_PIN, LOW); // Ensure trigger pin is low
  delayMicroseconds(2); // Wait for 2 microseconds
  digitalWrite(TRIG_PIN, HIGH); // Set trigger pin high
  delayMicroseconds(10); // Wait for 10 microseconds
  digitalWrite(TRIG_PIN, LOW); // Set trigger pin low

  // Measure the pulse duration
  unsigned long duration = pulseIn(ECHO_PIN, HIGH); // Read the duration of the pulse on the echo pin

  // Convert duration to distance
  return duration * 0.034 / 2; // Speed of sound is 343 m/s; convert to distance
}

void loop() {
  unsigned int distance = measureDistance(); // Measure the distance using the ultrasonic sensor

  // Create CAN message
  unsigned char data[2]; // Array to hold the CAN message data
  data[0] = distance >> 8; // Higher byte of the distance
  data[1] = distance & 0xFF; // Lower byte of the distance

  // Send CAN message
  CAN.sendMsgBuf(0x100, 0, 2, data); // Send the distance data via CAN with ID 0x100

  // Print to Serial Monitor
  Serial.print("Distance1: "); // Print the measured distance to the serial monitor
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000); // Wait for a second before the next measurement
}
