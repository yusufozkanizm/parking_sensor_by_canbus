#include <SPI.h>
#include <mcp_can.h>

// MCP2515 CAN Controller
const int CAN_CS_PIN = 10; // Chip Select pin for the CAN controller
const int CAN_INT_PIN = 2; // Interrupt pin for the CAN controller
MCP_CAN CAN(CAN_CS_PIN);

// LEDs and Buzzer
const int LED1_PIN = 7;  // LED for Sensor 1
const int LED2_PIN = 8;  // LED for Sensor 2
const int BUZZER_PIN = 9; // Buzzer for Sensor 1

unsigned int distance1 = 0; // Distance measured by Sensor 1
unsigned int distance2 = 0; // Distance measured by Sensor 2

// Timing variables for non-blocking flashing
unsigned long previousMillis1 = 0; // Store last time LED1 was updated
unsigned long previousMillis2 = 0; // Store last time LED2 was updated
unsigned long previousMillisBuzzer = 0; // Store last time Buzzer was updated

int interval1 = 1000; // Initial interval for LED1 and Buzzer (Sensor 1)
int interval2 = 1000; // Initial interval for LED2 (Sensor 2)

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

  // Initialize LEDs and Buzzer
  pinMode(LED1_PIN, OUTPUT); // Set LED1 pin as output
  pinMode(LED2_PIN, OUTPUT); // Set LED2 pin as output
  pinMode(BUZZER_PIN, OUTPUT); // Set Buzzer pin as output
  digitalWrite(LED1_PIN, LOW); // Ensure LED1 is off
  digitalWrite(LED2_PIN, LOW); // Ensure LED2 is off
  digitalWrite(BUZZER_PIN, LOW); // Ensure Buzzer is off
}

void displayDistances() {
  // Print distances to Serial Monitor
  Serial.print("S1: "); // Print sensor 1 distance label
  Serial.print(distance1); // Print distance measured by sensor 1
  Serial.print(" cm");
  Serial.print(" | S2: "); // Print sensor 2 distance label
  Serial.print(distance2); // Print distance measured by sensor 2
  Serial.println(" cm");
}

void updateFlashRates() {
  // Update intervals based on distance
  interval1 = map(distance1, 0, 100, 50, 1000); // Map distance to delay (closer = faster flash)
  interval2 = map(distance2, 0, 100, 50, 1000); // Map distance to delay (closer = faster flash)
}

void loop() {
  unsigned long currentMillis = millis(); // Get the current time

  if (CAN.checkReceive() == CAN_MSGAVAIL) { // Check if a CAN message is available
    unsigned long id; // CAN message ID
    unsigned char len; // Length of CAN message
    unsigned char data[8]; // Data of CAN message
    CAN.readMsgBuf(&id, &len, data); // Read the CAN message

    if (id == 0x100) { // If the message ID is 0x100 (Sensor 1)
      distance1 = (data[0] << 8) | data[1]; // Combine the two bytes to form the distance
    } else if (id == 0x101) { // If the message ID is 0x101 (Sensor 2)
      distance2 = (data[0] << 8) | data[1]; // Combine the two bytes to form the distance
    }

    displayDistances(); // Display distances on the Serial Monitor
    updateFlashRates(); // Update flash rates based on the distances
  }

  // Flash LED1 and Buzzer based on distance1
  if (distance1 <= 100) { // If distance1 is within 100 cm
    if (currentMillis - previousMillis1 >= interval1) { // Check if it's time to toggle LED1
      previousMillis1 = currentMillis; // Save the current time
      digitalWrite(LED1_PIN, !digitalRead(LED1_PIN)); // Toggle LED1
    }
    if (currentMillis - previousMillisBuzzer >= interval1) { // Check if it's time to toggle the Buzzer
      previousMillisBuzzer = currentMillis; // Save the current time
      digitalWrite(BUZZER_PIN, !digitalRead(BUZZER_PIN)); // Toggle the Buzzer
    }
  } else {
    digitalWrite(LED1_PIN, LOW); // Ensure LED1 is off
    digitalWrite(BUZZER_PIN, LOW); // Ensure Buzzer is off
  }

  // Flash LED2 based on distance2
  if (distance2 <= 100) { // If distance2 is within 100 cm
    if (currentMillis - previousMillis2 >= interval2) { // Check if it's time to toggle LED2
      previousMillis2 = currentMillis; // Save the current time
      digitalWrite(LED2_PIN, !digitalRead(LED2_PIN)); // Toggle LED2
    }
  } else {
    digitalWrite(LED2_PIN, LOW); // Ensure LED2 is off
  }
}
