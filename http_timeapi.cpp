#include "application.h"
#include <time.h>
#define DEBUG true

String http_get(const char* hostname, String path);
long currentTimeMilis();

int led = D7;

unsigned int nextTime = 0;    // next time to contact the server

void setup() {
	Serial.begin(9600);
	pinMode(led, OUTPUT);
}

void loop() {
	if (nextTime > millis()) {
		return;
	}

	digitalWrite(led, HIGH);   // Turn ON the LED
	delay(500);               // Wait for 1000mS = 1 second
	digitalWrite(led, LOW);    // Turn OFF the LED
	delay(500);               // Wait for 1 second

	long timestamp = currentTimeMilis();
	Serial.begin(9600);

	nextTime = millis() + 10000;
}

TCPClient client;
char buffer[1024];

/*timeapi example :
 *
 GET /utc/now HTTP/1.0
 Host: www.timeapi.org
 Connection: close

 HTTP/1.1 200 OK
 Server: nginx
 Date: Sat, 11 Jan 2014 11:59:37 GMT
 Content-Type: text/html;charset=utf-8
 Content-Length: 25
 Connection: keep-alive
 X-Frame-Options: sameorigin
 X-Xss-Protection: 1; mode=block

 2014-01-11T11:59:37+00:00
 */

String http_get(char const* hostname, String path) {

	if (client.connect(hostname, 80)) {
		if (DEBUG)
			Serial.println("connected");
		client.print("GET ");
		client.print(path);
		client.print(" HTTP/1.0\n");
		client.print("HOST: ");
		client.println(hostname);
		client.print("\n");
		//	client.print("Connection: close\n\n");
		client.flush();
	} else {
		Serial.println("connection failed");
		client.stop();
		return NULL;
	}

	// Block until first byte is read.
	client.read();

	if (DEBUG)
		Serial.println("reading response....");
	for (unsigned int i = 0; i < sizeof(buffer) && client.available(); i++) {
		char c = client.read();
		if (c == -1) {
			break;
		}
		buffer[i] = c;
	}

	client.stop();

	String response(buffer);
	int bodyPos = response.indexOf("\r\n\r\n");
	if (bodyPos != -1) {
		Serial.println("found http body:");
		bodyPos += 4; // skip RNRN
	} else {
		Serial.println("can not find http reponse body");
		return NULL;
	}
	response = response.substring(bodyPos);
	return response;
}

long parseDate(String str) {
	// make sure it works on UTC
	setenv("TZ", "UTC", 1);
	tzset();                // recognize TZ

	// parse date
	struct tm t1;
	strptime(str.c_str(), "%Y-%m-%dT%H:%M:%S", &t1);
	return (long) mktime(&t1);
}

long parseTzOffset(String str) {
	// strptime currently does not parse the timezone with %z, so we do it ourself:
	// parse 3 digits the "+0100" which result in 1 hour.
	int offsetHours;
	sscanf(str.c_str(), "%*19s%3d", &offsetHours);
	return offsetHours * 3600;
}

long currentTimeMilis() {
	Serial.println("getting current time");

	String response = http_get("www.timeapi.org", "/utc/now");
	if (response != NULL) {
		Serial.print("time=");
		Serial.println(response);
		return parseDate(response);
	} else {
		return 0;
	}
}
