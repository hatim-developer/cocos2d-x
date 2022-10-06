#import "platform/ios/CCLocalStorage-ios.h"
#import "platform/CCPlatformMacros.h"

#pragma mark-
#pragma mark UserDefaults Helper

@interface UserDefaultsHelper : NSObject
+ (void) saveToUserDefaults:(NSString*) key withValue:(id) value;
+ (id) retrieveFromUserDefaults:(NSString*)key;
+ (void) removeFromUserDefaults: (NSString*) key;
+ (void) clearUserDefaults;
+ (void) syncUserDefaults;
@end

@implementation UserDefaultsHelper
+ (void) saveToUserDefaults:(NSString*) key withValue:(id) value
{
    NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];

    if (standardUserDefaults) {
        [standardUserDefaults setObject:value forKey:key];
    }
}

+ (id) retrieveFromUserDefaults:(NSString*)key
{
    NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
    id val = nil;

    if (standardUserDefaults) {
        val = [standardUserDefaults objectForKey:key];
    }
    return val;
}

+ (void) removeFromUserDefaults: (NSString*) key
{
    NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];

    if (standardUserDefaults) {
        [standardUserDefaults removeObjectForKey:key];
    }
}

+ (void) clearUserDefaults
{
    NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
    if (standardUserDefaults) {
        [standardUserDefaults setPersistentDomain:[NSDictionary dictionary] forName:[[NSBundle mainBundle] bundleIdentifier]];
    }
}

+ (void) syncUserDefaults
{
    NSUserDefaults *standardUserDefaults = [NSUserDefaults standardUserDefaults];
    if (standardUserDefaults) {
        [standardUserDefaults synchronize];
    }
}
@end

using namespace std;

NS_CC_BEGIN

#pragma mark-
#pragma mark Cpp Wrapper

void LocalStorageIos::setItem(const std::string& key, const std::string &value)
{
    NSMutableDictionary *storage = [[NSMutableDictionary dictionaryWithDictionary:(NSMutableDictionary*)
                                     [UserDefaultsHelper retrieveFromUserDefaults:@"localStorage"]] retain];

    if (!storage) {
        storage = [[NSMutableDictionary alloc] init];
    }

    [storage setObject:[NSString stringWithUTF8String: value.c_str()]
        forKey:[NSString stringWithUTF8String:key.c_str()]];

    [UserDefaultsHelper saveToUserDefaults:@"localStorage" withValue:storage];
    [storage release];
}

bool LocalStorageIos::getItem(const std::string &key, std::string *outItem)
{
    try {
        NSMutableDictionary* storage = (NSMutableDictionary*) [UserDefaultsHelper retrieveFromUserDefaults:@"localStorage"];

        if (!storage) {
            return false;
        }

        NSString *str = [storage objectForKey:[NSString stringWithUTF8String:key.c_str()]];

        if (!str) {
            return false;
        }

        outItem->assign((const char*) [str UTF8String]);
        return true;
    } catch (NSException* e) {
        NSLog(@"{LocalStorageIos} getItem Error key = %@ exception = %@", [NSString stringWithUTF8String:key.c_str()], e);
        return false;
    }
}

void LocalStorageIos::removeItem(const std::string& key)
{
    NSMutableDictionary *storage = [NSMutableDictionary dictionaryWithDictionary:(NSMutableDictionary*)
                                    [UserDefaultsHelper retrieveFromUserDefaults:@"localStorage"]];
    if (storage) {
        [storage removeObjectForKey:[NSString stringWithUTF8String:key.c_str()]];
        [UserDefaultsHelper saveToUserDefaults:@"localStorage" withValue:storage];
    }
}

void LocalStorageIos::clear()
{
    NSMutableDictionary *storage = [NSMutableDictionary dictionaryWithDictionary:(NSMutableDictionary*)
                                    [UserDefaultsHelper retrieveFromUserDefaults:@"localStorage"]];
    if (storage) {
        [storage removeAllObjects];
        [UserDefaultsHelper saveToUserDefaults:@"localStorage" withValue:storage];
    }
}

#pragma mark-
#pragma mark Init / Destroy

LocalStorageIos* LocalStorageIos::_instance = nullptr;

LocalStorageIos* LocalStorageIos::getInstance()
{
    if (!_instance) {
        _instance = new (std::nothrow)LocalStorageIos();
    }

    return _instance;
}

void LocalStorageIos::destroyInstance()
{
    CC_SAFE_DELETE(_instance);
}

LocalStorageIos:: LocalStorageIos()
{
}

LocalStorageIos::~LocalStorageIos()
{
}
NS_CC_END
