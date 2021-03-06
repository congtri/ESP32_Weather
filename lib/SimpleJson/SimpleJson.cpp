#include "SimpleJson.h"
#include <string>

#define PADDING			(2) /* This is size of charactor :" */


SimpleJson::SimpleJson(/* args */)
{
	this->json_value.length = 0;
	this->json_value.data = 0;
	this->json_string = nullptr;
}

SimpleJson::SimpleJson(String *json)
{
	this->json_value.length = 0;
	this->json_value.data = 0;
	this->json_string = json;
}

SimpleJson::~SimpleJson()
{
}

void SimpleJson::initJsonString(String *json)
{
    this->json_string = json;
}

int SimpleJson::findJsonValue(String key)
{
	int s_pos = 0, e_pos = 0;
    int error = 0;

	/* Find the key of json */
    String *json = this->json_string;
	const char *c_json = this->json_string->c_str();

	s_pos = json->indexOf(key);

	if (s_pos != 0)//string::npos)
	{
		s_pos = s_pos + key.length() + PADDING;
		e_pos = s_pos + 1;

		if (json->charAt(s_pos) == '"')
		{
			s_pos++;
			e_pos++;
		}

		/* Loking for the end position of json string value */
		for (; e_pos < json->length(); e_pos++)
		{
			if (json->charAt(e_pos) == '\n' || json->charAt(e_pos) == ',' || json->charAt(e_pos) == '"')
			{
				this->json_value.data = (char *)&c_json[s_pos];
				this->json_value.length = e_pos - s_pos;
				
                error = 0;
                break;
			}
		}

        error = 1;
	}
    else
    {
        error = 1;
    }

	return error;
}

int SimpleJson::findJsonValue(String &json_data, String key)
{
	int s_pos = 0, e_pos = 0;
    int error = 0;

	/* reinit the json string */
	this->json_string = &json_data;

	/* Find the key of json */
    String *json = this->json_string;

	s_pos = json->indexOf(key);

	if (s_pos != 0)//string::npos)
	{
		s_pos = s_pos + key.length() + PADDING;
		e_pos = s_pos + 1;

		if (json->charAt(s_pos) == '"')
		{
			s_pos++;
			e_pos++;
		}

		/* Loking for the end position of json string value */
		for (; e_pos < json->length(); e_pos++)
		{
			if (json->charAt(e_pos) == '\n' || json->charAt(e_pos) == ',' || json->charAt(e_pos) == '"')
			{
				//this->json_value.data = &json[s_pos];
				this->json_value.data = &json_data[s_pos];
				this->json_value.length = e_pos - s_pos;
				
                error = 0;
                break;
			}
		}

        error = 1;
	}
    else
    {
        error = 1;
    }

	return error;
}

void SimpleJson::showJsonValueData()
{
	if (this->json_value.data != nullptr)
	{
		Serial.print("\nParsed data: ");
		int i;
		for (i = 0; i < this->json_value.length ; i++)
		{
			Serial.printf("%c", this->json_value.data[i]);
		}
		Serial.printf(" - lenght: %d\n", this->json_value.length);
	}
	else
	{
		Serial.print("\nData is empty\n");
	}
}