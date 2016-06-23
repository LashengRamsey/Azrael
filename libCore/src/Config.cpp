#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <string>
#include <iostream>
#include "Config.h"


bool Config::mFlag = false;
std::map<std::string,std::string> Config::m_KeyValueMap;
char* Config::mConfigName = "";
char* Config::mCommConfig = "";


Config::Config()
{
}

Config::~Config(void)
{
	m_KeyValueMap.clear();
}

bool InStr(char ch,const char *pStr)
{
  for(int i = 0;pStr[i] != '\0';++i)
  {
  	if(ch == pStr[i])
  	  return true;
  }
  return false;
}

void Config::SetConfigName(char* configName, char* commConfig)
{
	mConfigName = configName;
	mCommConfig = commConfig;
}

bool Config::ReadFromFile(const char *pFilePath)
{
	if (strlen(pFilePath) == 0)
		return false;
	FILE* fp = fopen(pFilePath,"rb");
	if( fp )
	{
		fseek(fp,0,SEEK_END);
		long t = ftell(fp);	
		if(t > 0)
		{
			char *pBuf = new char[t];
			fseek(fp,0,SEEK_SET);
			fread(pBuf,t,1,fp);		
			ReadFromBuffer(pBuf,t);
			delete [] pBuf;
		}
		fclose(fp);
		return true;
	}
	return false;
}

void Config::ReadFromBuffer(char *pBuf,long uSize)
{
	bool b = true;
	bool bNote = false; 
	char key[128] = "";
	char value[128] = "";
	char chs[2] = {0,0};

	for(int i = 0;i < uSize && pBuf[i] != '\0'; ++i)
	{
		chs[0] = pBuf[i];
		if ( InStr(chs[0],"#") || bNote)
		{
			bNote = true;
			if ( InStr(chs[0],"\r\n"))
			{
				bNote = false;
			}
			continue;
		}
		if(InStr(chs[0]," \r\n\t=")) 
		{
			if( b )
			{
				if(strcmp(key,"") != 0) 
					b = false;
			}
			else
			{
				if(strcmp(value,"")!=0)
				{
					b = true;
					std::cout << "Config:		" << key << "=" << value << std::endl;
					m_KeyValueMap[key]=value;
					strcpy(key,"");
					strcpy(value,"");
				}
			}
		}
		else
		{
			if( b )
				strcat(key,chs);  //key
			else 
				strcat(value,chs);  //value
		}
	}
	
	
	if( strcmp(key,"") != 0 && strcmp(value,"") !=0 )
	{
		m_KeyValueMap[key] = value;
	}
}

void Config::PrintAllConfig()
{
  KeyValueMap::iterator it;
  for (it = m_KeyValueMap.begin(); it != m_KeyValueMap.end(); it++ )
  {
	  //printf("Config:%s->%s",it->first,it->second);
    //LOG_INFO << "Config:" << it->first << ":" << it->second;
  }
}


const char * Config::GetValue(const char * strKey)
{
	if (! Config::mFlag)
	{
		Config::mFlag = true;
		m_KeyValueMap.clear();
		ReadFromFile(mConfigName);
		ReadFromFile(mCommConfig);
	}

	KeyValueMap::iterator it = m_KeyValueMap.find(strKey);
	if( it != m_KeyValueMap.end())
	{
		return it->second.c_str();
	}
	return "";
}

int Config::GetIntValue(const char * strKey)
{
	std::string sValue = GetValue(strKey);
	if( !sValue.empty() )
	{
		return atoi(sValue.c_str());
	}
	return 0;
}

