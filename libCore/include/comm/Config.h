#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>

//≈‰÷√Œƒº˛£∫GameConfig.ini
class Config
{
public:
	Config();
	~Config();

	static void PrintAllConfig();
	static const char* GetValue(const char* strKey);
	static int GetIntValue(const char* strKey);
	static void SetConfigName(char* configName, char* commConfig);
protected:
	static bool ReadFromFile(const char *pFilePath);
	static void ReadFromBuffer(char *pBuf,long uSize);


private:
	static bool mFlag;
	static char *mConfigName;
	static char *mCommConfig;
	typedef std::map<std::string, std::string> KeyValueMap;
	static KeyValueMap m_KeyValueMap; 
};

#endif  //CONFIG_H
