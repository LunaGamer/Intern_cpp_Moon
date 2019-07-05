#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>
#include <fstream>

using namespace rapidjson;


int main() {
	char* name = new char[256];
	char* info = new char[256];
	char* info1 = new char[256];
	char* info2 = new char[256];
	char* value = new char[256];
	int age;
	int maxAge = 0, minAge = 0;
	Value bff;
	Value obj(kObjectType);
	char menu = NULL;
	bool error;
	FILE* fp = fopen(PATH_TO_JSON, "r");
	fseek(fp, 0, SEEK_END);
	size_t filesize = (size_t)ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* buf = (char*)malloc(filesize + 1);
	size_t readLength = fread(buf, 1, filesize, fp);
	buf[readLength] = '\0';
	fclose(fp);
	Document d;
	d.Parse(buf);
	do
	{
		menu = NULL;

		StringBuffer buffer;
		Writer<StringBuffer> writer(buffer);
		d.Accept(writer);
		const Value& itemn = d;
		std::cout << "1 - Search\n2 - Add\n3 - Delete\n4 - MinMax Age\n5 - Change Value\n6 - show JSON string\nq - Exit\n";
		do
		{
			error = false;
			std::cin >> menu;
			if (menu != '1' && menu != '2' && menu != '3' && menu != '4' && menu != '5' && menu != '6' && menu != 'q')
			{
				error = true;
				std::cout << "ERROR!!! TRY AGAIN\n";
			}
		} while (error);
		switch (menu)
		{
		case '1':
			system("CLS");
			std::cout << "enter name\n";
			std::cin >> name;
			std::cout << "enter info\n";
			std::cin >> info;
			if (d.HasMember(name) && d[name].HasMember(info))
			{
				if (d[name][info].IsInt())
				{
					std::cout << name << "'s " << info << ": " << d[name][info].GetInt() << std::endl;
				}
				else
				{
					std::cout << name << "'s " << info << ": " << d[name][info].GetString() << std::endl;
				}
			}
			else {
				std::cout << "Error\n";
			}
			break;
		case '2':
			system("CLS");
			std::cout << "Adding new Member\nEnter name:\n";
			std::cin >> name;
			std::cout << "Enter username:\n";
			std::cin >> info1;
			std::cout << "Enter location:\n";
			std::cin >> info2;
			std::cout << "Enter age:\n";
			std::cin >> age;
			bff.SetString(info1, d.GetAllocator());
			obj.AddMember("username", bff, d.GetAllocator());
			bff.SetString(info2, d.GetAllocator());
			obj.AddMember("location", bff, d.GetAllocator());
			obj.AddMember("age", age, d.GetAllocator());
			bff.SetString(name, d.GetAllocator());
			d.AddMember(bff, obj, d.GetAllocator());
			break;
		case '3':
			system("CLS");
			std::cout << "enter name\n";
			std::cin >> name;
			if (d.HasMember(name))
			{
				bff.SetString(name, d.GetAllocator());
				d.RemoveMember(name);
			}
			else {
				std::cout << "Error\n";
			}
			break;
		case '4':
			system("CLS");
			for (Value::ConstMemberIterator itr = itemn.MemberBegin(); itr != itemn.MemberEnd(); ++itr)
			{
				age = d[itr->name.GetString()]["age"].GetInt();
				if (maxAge == 0 || minAge == 0)
				{
					maxAge = age;
					minAge = age;
				}
				else
				{
					if (age > maxAge) maxAge = age;
					if (age < minAge) minAge = age;
				}
			}
			std::cout << "min age: " << minAge << "\nmax age: " << maxAge << std::endl;
			break;
		case '5':
			system("CLS");
			std::cout << "enter name\n";
			std::cin >> name;
			std::cout << "enter info to change\n";
			std::cin >> info;
			if (d.HasMember(name) && d[name].HasMember(info))
			{
				if (d[name][info].IsInt())
				{
					std::cout << "enter value\n";
					std::cin >> age;
					Value& s = d[name][info];
					s.SetInt(age);
				}
				else
				{
					std::cout << "enter value\n";
					std::cin >> value;
					Value& s = d[name][info];
					s.SetString(value, d.GetAllocator());
				}
			}
			else {
				std::cout << "Error\n";
			}
			break;
		case '6':
			system("CLS");
			std::cout << "JSON string:\n" << buffer.GetString() << std::endl;
			break;
		case 'q':
			std::cout << "Exit!\nfinal json string:\n";
			break;
		default:
			break;
		}
	} while (menu != 'q');
	StringBuffer buffer3;
	Writer<StringBuffer> writer2(buffer3);
	d.Accept(writer2);
	std::cout << buffer3.GetString() << std::endl << "rewrite json file?\ny - yes, n - no\n";
	do
	{
		error = false;
		std::cin >> menu;
		if (menu != 'y' && menu != 'n')
		{
			error = true;
			std::cout << "ERROR!!! TRY AGAIN\n";
		}
	} while (error);
	if (menu == 'y') 
	{
		std::string json(buffer3.GetString(), buffer3.GetSize());
		std::ofstream ofile(PATH_TO_JSON);
		ofile.clear();
		ofile << json;
		ofile.close();
	}
	return 0;
}