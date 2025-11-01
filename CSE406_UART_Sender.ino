#define TXD1 19
#define RXD1 21
HardwareSerial mySerial(1); // UART1 for board-to-board
int baudRates[] = {9600, 38400, 115200};
int msgSizes[] = {10, 50, 100};
int intervals[] = {0, 10, 100};
int testDuration = 10000; // 10 seconds per test

void setup() {
  Serial.begin(38400);
  delay(1000);
  Serial.println("ESP32 UART Stress Test — Sender");
}

void loop() {
  for (int b = 0; b < 3; b++) {
    for (int s = 0; s < 3; s++) {
      for (int i = 0; i < 3; i++) {

        mySerial.begin(baudRates[b], SERIAL_8N1, RXD1, TXD1);
        delay(100);
        Serial.printf("\n[TEST] Baud=%d, Size=%d, Interval=%dms\n",
                      baudRates[b], msgSizes[s], intervals[i]);

        unsigned long startTime = millis();
        unsigned long sent = 0;

        // Prepare message of required size
        String msg = "";
        for (int j = 0; j < msgSizes[s]; j++) msg += char('A' + (j % 26));

        while (millis() - startTime < testDuration) {
          mySerial.println(msg);
          sent++;
          delay(intervals[i]);
        }

        mySerial.flush();
        Serial.printf("Sent %lu messages\n", sent);
        delay(2000); // wait before next test
      }
    }
  }

  Serial.println("All tests complete!");
  while (1);
}