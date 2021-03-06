#include "SimpleJson.h"
#include "string.h"

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
				this->json_value.data = (char *)&json[s_pos];
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