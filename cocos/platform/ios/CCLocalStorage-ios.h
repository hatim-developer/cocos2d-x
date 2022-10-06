/* CCLocalStorage-ios.h: Local Storage using user-defaults support for iOS. */

#ifndef _CC_LOCAL_STORAGE_IOS_H_
#define _CC_LOCAL_STORAGE_IOS_H_

#include <string>
#include "platform/CCPlatformMacros.h"

NS_CC_BEGIN

class LocalStorageIos {
public:
    static LocalStorageIos* getInstance();
    static void destroyInstance();
    void setItem(const std::string& key, const std::string &value);
    bool getItem(const std::string& key, std::string *outItem);
    void removeItem(const std::string& key);
    void clear();

protected:
    LocalStorageIos();
    virtual ~LocalStorageIos();

private:
    static LocalStorageIos* _instance;
};

NS_CC_END

#endif // _CC_LOCAL_STORAGE_IOS_H_
