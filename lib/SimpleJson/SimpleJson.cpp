#include "SimpleJson.h"

SimpleJson::SimpleJson()
{
	this->json_string = nullptr;
}

SimpleJson::SimpleJson(String *json)
{
	this->json_string = json;
}

SimpleJson::~SimpleJson()
{
}

bool SimpleJson::setJsonData(String *json)
{
	if (json == nullptr) {
		return false;
	}
	this->json_string = json;
	return true;
}

// Helper method to find proper key position in JSON
int SimpleJson::findKeyPosition(String &json, String key)
{
	String search_pattern = "\"" + key + "\"";
	int pos = 0;
	
	while (pos < json.length()) {
		int found = json.indexOf(search_pattern, pos);
		if (found == -1) {
			return -1; // Key not found
		}
		
		// Check if this is a valid key (not part of a value)
		// Look for opening brace or comma before the key
		if (found == 0 || json.charAt(found - 1) == '{' || json.charAt(found - 1) == ',') {
			return found + 1; // Return position after opening quote
		}
		
		// Check for whitespace before key (valid JSON)
		int check_pos = found - 1;
		while (check_pos >= 0 && (json.charAt(check_pos) == ' ' || json.charAt(check_pos) == '\t' || 
		       json.charAt(check_pos) == '\n' || json.charAt(check_pos) == '\r')) {
			check_pos--;
		}
		if (check_pos >= 0 && (json.charAt(check_pos) == '{' || json.charAt(check_pos) == ',')) {
			return found + 1; // Return position after opening quote
		}
		
		pos = found + 1;
	}
	
	return -1;
}

bool SimpleJson::parseValue(String key)
{
	if (this->json_string == nullptr || key.length() == 0) {
		return false; // Error: null pointer or empty key
	}

	int key_pos = findKeyPosition(*this->json_string, key);
	if (key_pos == -1) {
		return false; // Error: key not found
	}

	// Move to position after key and look for colon
	int pos = key_pos + key.length();
	
	// Skip whitespace and find colon
	while (pos < this->json_string->length() && 
	       (this->json_string->charAt(pos) == ' ' || this->json_string->charAt(pos) == '\t')) {
		pos++;
	}
	
	if (pos >= this->json_string->length() || this->json_string->charAt(pos) != ':') {
		return false; // Error: no colon found
	}
	pos++; // Skip colon
	
	// Skip whitespace after colon
	while (pos < this->json_string->length() && 
	       (this->json_string->charAt(pos) == ' ' || this->json_string->charAt(pos) == '\t')) {
		pos++;
	}
	
	if (pos >= this->json_string->length()) {
		return false; // Error: no value found
	}
	
	int start_pos = pos;
	bool is_string = false;
	
	// Check if value is a string (starts with quote)
	if (this->json_string->charAt(pos) == '"') {
		is_string = true;
		start_pos = ++pos; // Skip opening quote
		
		// Find closing quote
		while (pos < this->json_string->length()) {
			if (this->json_string->charAt(pos) == '"' && 
			    (pos == start_pos || this->json_string->charAt(pos - 1) != '\\')) {
				break;
			}
			pos++;
		}
	} else {
		// Find end of non-string value
		while (pos < this->json_string->length()) {
			char c = this->json_string->charAt(pos);
			if (c == ',' || c == '}' || c == ']' || c == '\n' || c == '\r' || c == ' ' || c == '\t') {
				break;
			}
			pos++;
		}
	}
	
	// Extract value to safe storage
	this->temp_value = this->json_string->substring(start_pos, pos);
	this->temp_value.trim(); // Remove any trailing whitespace for non-strings
	
	return true; // Success
}

bool SimpleJson::parseValueFromJson(String &json_data, String key)
{
	if (key.length() == 0) {
		return false; // Error: empty key
	}

	// Temporarily set json_string to point to provided data
	String *old_json = this->json_string;
	this->json_string = &json_data;

	int key_pos = findKeyPosition(json_data, key);
	if (key_pos == -1) {
		this->json_string = old_json; // Restore original
		return false; // Error: key not found
	}

	// Move to position after key and look for colon
	int pos = key_pos + key.length();
	
	// Skip whitespace and find colon
	while (pos < json_data.length() && 
	       (json_data.charAt(pos) == ' ' || json_data.charAt(pos) == '\t')) {
		pos++;
	}
	
	if (pos >= json_data.length() || json_data.charAt(pos) != ':') {
		this->json_string = old_json; // Restore original
		return false; // Error: no colon found
	}
	pos++; // Skip colon
	
	// Skip whitespace after colon
	while (pos < json_data.length() && 
	       (json_data.charAt(pos) == ' ' || json_data.charAt(pos) == '\t')) {
		pos++;
	}
	
	if (pos >= json_data.length()) {
		this->json_string = old_json; // Restore original
		return false; // Error: no value found
	}
	
	int start_pos = pos;
	bool is_string = false;
	
	// Check if value is a string (starts with quote)
	if (json_data.charAt(pos) == '"') {
		is_string = true;
		start_pos = ++pos; // Skip opening quote
		
		// Find closing quote
		while (pos < json_data.length()) {
			if (json_data.charAt(pos) == '"' && 
			    (pos == start_pos || json_data.charAt(pos - 1) != '\\')) {
				break;
			}
			pos++;
		}
	} else {
		// Find end of non-string value
		while (pos < json_data.length()) {
			char c = json_data.charAt(pos);
			if (c == ',' || c == '}' || c == ']' || c == '\n' || c == '\r' || c == ' ' || c == '\t') {
				break;
			}
			pos++;
		}
	}
	
	// Extract value to safe storage
	this->temp_value = json_data.substring(start_pos, pos);
	this->temp_value.trim(); // Remove any trailing whitespace for non-strings
	
	// Restore original json_string
	this->json_string = old_json;
	
	return true; // Success
}

void SimpleJson::printParsedValue()
{
	if (this->temp_value.length() > 0)
	{
		Serial.print("\nParsed data: ");
		Serial.print(this->temp_value);
		Serial.print(" - length: ");
		Serial.println(this->temp_value.length());
	}
	else
	{
		Serial.println("\nData is empty");
	}
}

bool SimpleJson::convertToInt(int &result)
{
	if (this->temp_value.length() == 0) {
		return false;
	}
	
	// Check if string contains only valid integer characters
	bool valid = true;
	int start = 0;
	if (this->temp_value.length() > 0 && (this->temp_value.charAt(0) == '-' || this->temp_value.charAt(0) == '+')) {
		start = 1;
	}
	
	for (int i = start; i < this->temp_value.length(); i++) {
		if (!isDigit(this->temp_value.charAt(i))) {
			valid = false;
			break;
		}
	}
	
	if (valid && this->temp_value.length() > start) {
		result = this->temp_value.toInt();
		return true;
	}
	
	return false;
}

bool SimpleJson::convertToLongLong(long long &result)
{
	if (this->temp_value.length() == 0) {
		return false;
	}
	
	// Check if string contains only valid integer characters
	bool valid = true;
	int start = 0;
	if (this->temp_value.length() > 0 && (this->temp_value.charAt(0) == '-' || this->temp_value.charAt(0) == '+')) {
		start = 1;
	}
	
	for (int i = start; i < this->temp_value.length(); i++) {
		if (!isDigit(this->temp_value.charAt(i))) {
			valid = false;
			break;
		}
	}
	
	if (valid && this->temp_value.length() > start) {
		// Use atoll with null-terminated string
		char temp_buffer[32];
		this->temp_value.toCharArray(temp_buffer, sizeof(temp_buffer));
		result = atoll(temp_buffer);
		return true;
	}
	
	return false;
}

bool SimpleJson::convertToFloat(float &result)
{
	if (this->temp_value.length() == 0) {
		return false;
	}
	
	// Check if string contains only valid float characters
	bool valid = true;
	bool has_decimal = false;
	int start = 0;
	if (this->temp_value.length() > 0 && (this->temp_value.charAt(0) == '-' || this->temp_value.charAt(0) == '+')) {
		start = 1;
	}
	
	for (int i = start; i < this->temp_value.length(); i++) {
		char c = this->temp_value.charAt(i);
		if (isDigit(c)) {
			continue;
		} else if (c == '.' && !has_decimal) {
			has_decimal = true;
		} else {
			valid = false;
			break;
		}
	}
	
	if (valid && this->temp_value.length() > start) {
		result = this->temp_value.toFloat();
		return true;
	}
	
	return false;
}

bool SimpleJson::hasValidData()
{
	return this->temp_value.length() > 0;
}

int SimpleJson::getValueLength()
{
	return this->temp_value.length();
}

String SimpleJson::getValueAsString()
{
	return this->temp_value;
}
