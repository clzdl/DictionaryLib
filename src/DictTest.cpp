/*
 * DictTest.cpp
 *
 *  Created on: 2017年12月24日
 *      Author: cplusplus
 */


#include "DictElement.h"
#include "DictPropertyManager.h"
#include "fstream"

int main(int argc, char* argv[])
{
	try
	{
		DictionaryLib::ElementManager::Instance("../conf/dict-config.xml");
		DictionaryLib::ElementManager::Instance()->DebugDump();

		std::unique_ptr<DictionaryLib::DictPropertyManager> property = DictionaryLib::DictPropertyManager::Create();
		property->SetFieldValueByPath("Session-Id", "session-id-123123");
		property->SetFieldValueByPath("Origin-Host", "origin-host-12123123");
		property->SetFieldValueByPath("Subscription-Id.Subscription-Id-Type", "Subscription-Id-Type-12123123");
		property->SetFieldValueByPath("Subscription-Id.Subscription-Id-Data", "Subscription-Id-Data-12123123");
		fprintf(stdout , "///////////////////////////////////\n");
		property->DebugDump();
		fprintf(stdout , "///////////////////////////////////\n");
		char buffer[8096] = {0};
		int len = property->Encode(buffer);

		std::ofstream of("1111111111.dat",std::ios_base::binary);
		of.write(buffer , len);
	}
	catch(DictionaryLib::DictionaryException &e)
	{
		fprintf(stdout , e.toString().c_str());
	}
}

