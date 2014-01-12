#include "application.h"

// ------------- HTTP functions --------------

/**
 * make http request and return body
 */
TCPClient client;
char buffer[512];
String http_get(char const* hostname, String path) {

	if (client.connect(hostname, 80)) {
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
	if (bodyPos == -1) {
		Serial.println("can not find http reponse body");
		return NULL;
	}
	return response.substring(bodyPos + 4);
}

// ------------- DATE / TIME functions --------------

/**
 * returns current time since epoche, from a http server.
 */
long currentUnixTimestamp() {
	Serial.println("getting current time");

	String response = http_get("www.timeapi.org", "/utc/now?\\s");
	if (response != NULL) {
		Serial.print("timeapi time=");
		Serial.println(response);
		return atoi(response.c_str());
	} else {
		return 0;
	}
}

/*timeapi example :
 *
 GET /utc/now?\s HTTP/1.0
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

1389485095
 */



void setup() {
	Serial.begin(9600);
}

unsigned int nextTime = 0;    // next time to contact the server
void loop() {
	if (nextTime > millis()) {
		return;
	}

	long timestamp = currentUnixTimestamp();
	Serial.print("time: ");
	Serial.println(timestamp);

	nextTime = millis() + 10000;
}

