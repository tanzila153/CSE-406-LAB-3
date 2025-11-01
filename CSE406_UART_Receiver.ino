#define TXD2 19
#define RXD2 21
HardwareSerial mySerial(2); // UART2
int baudRates[] = {9600, 38400, 115200};
int msgSizes[] = {10, 50, 100};
int intervals[] = {0, 10, 100};
int testDuration = 10000; // 10 seconds per test

void setup() {
  Serial.begin(38400);
  delay(1000);
  Serial.println("ESP32 UART Stress Test — Receiver");
}

void loop() {
  for (int b = 0; b < 3; b++) {
    for (int s = 0; s < 3; s++) {
      for (int i = 0; i < 3; i++) {

        mySerial.begin(baudRates[b], SERIAL_8N1, RXD2, TXD2);
        delay(100);
        Serial.printf("\n[TEST] Baud=%d, Size=%d, Interval=%dms\n",
                      baudRates[b], msgSizes[s], intervals[i]);

        unsigned long startTime = millis();
        unsigned long recvCount = 0;
        unsigned long bytesReceived = 0;

        while (millis() - startTime < testDuration) {
          if (mySerial.available()) {
            String msg = mySerial.readStringUntil('\n');
            recvCount++;
            bytesReceived += msg.length();
          }
        }

        // Compute metrics
        float durationSec = testDuration / 1000.0;
        float throughput = bytesReceived / durationSec;
        float msgRate = recvCount / durationSec;

        // --- New: Estimate sent messages and calculate error rate ---
        // Sender sends one message every 'intervals[i]' ms (0 treated as no delay)
        unsigned long sentCount;
        if (intervals[i] == 0)
          sentCount = testDuration / 1; // fast loop, approximate 1 ms per send
        else
          sentCount = testDuration / intervals[i];

        if (sentCount == 0) sentCount = 1; // avoid division by zero
        float errorRate = 100.0 * (sentCount - recvCount) / sentCount;
        if (errorRate < 0) errorRate = 0;

        Serial.printf("Results: Baud=%d, Size=%d, Interval=%d, Bytes=%lu, Msgs=%lu, "
                      "Throughput=%.2f B/s, MsgRate=%.2f msg/s, Error=%.2f%%\n",
                      baudRates[b], msgSizes[s], intervals[i],
                      bytesReceived, recvCount, throughput, msgRate, errorRate);

        delay(2000);
      }
    }
  }

  Serial.println("All tests complete!");
  while (1);
}
