#include "application.h"
#include <time.h>

// ------------- DATE / TIME functions --------------
/**
 * parse a string of the form "2014-01-11T17:17:59+0200"
 */
long parseDate(String str) {
	// TODO: it assumes it is running in UTC. mktime() uses the local time (time zone) for creating timestamp.
	// parse date. timegm() would be better, but is not available.
	struct tm time;
	strptime(str.c_str(), "%Y-%m-%dT%H:%M:%S", &time);
	return (long) mktime(&time);
}

/**
 * can parse the timezone offset in the string "2014-01-11T17:17:59+0100"
 */
long parseTzOffset(String str) {
	// strptime currently does not parse the timezone with %z, so we do it ourself:
	// parse 3 digits the "+0100" which result in 1 hour.
	int offsetHours;
	sscanf(str.c_str(), "%*19s%3d", &offsetHours);
	return offsetHours * 3600;
}

void setup() {
	Serial.begin(9600);
}

unsigned int nextTime = 0;    // next time to run the code
void loop() {
	if (nextTime > millis()) {
		return;
	}

	long timestamp = parseDate("2014-01-11T17:17:59+0100");
	long offset = parseTzOffset("2014-01-11T17:17:59+0100");
	timestamp -= offset;

	Serial.print("timestamp: ");
	Serial.println(timestamp);

	if (timestamp == 1389457079) {
		Serial.println("SUCCESS");
	} else {
		Serial.println("FAIL, timestamp is incorrect");
	}

	nextTime = millis() + 10000;
}

