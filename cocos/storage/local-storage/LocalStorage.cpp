/*
 
 Copyright (c) 2012 - Zynga Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 */

/*
 Local Storage support for the JS Bindings for iOS.
 Works on cocos2d-iphone and cocos2d-x.
 */

#include "storage/local-storage/LocalStorage.h"
#include "platform/CCPlatformMacros.h"

#if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    // Comment below line to disable User Deafualts Usage for Local Storage
    #define USE_USER_DEFAULT_LS
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#ifdef USE_USER_DEFAULT_LS
#include "platform/ios/CCLocalStorage-ios.h"
#else
#include <sqlite3.h>

static sqlite3 *_db;
static sqlite3_stmt *_stmt_select;
static sqlite3_stmt *_stmt_remove;
static sqlite3_stmt *_stmt_update;
static sqlite3_stmt *_stmt_clear;

static void localStorageCreateTable()
{
    const char *sql_createtable = "CREATE TABLE IF NOT EXISTS data(key TEXT PRIMARY KEY,value TEXT);";
    sqlite3_stmt *stmt;
    int ok = sqlite3_prepare_v2(_db, sql_createtable, -1, &stmt, nullptr);
    ok |= sqlite3_step(stmt);
    ok |= sqlite3_finalize(stmt);
	
    if (ok != SQLITE_OK && ok != SQLITE_DONE)
        printf("Error in CREATE TABLE\n");
}

#endif

static int _initialized = 0;

void localStorageInit( const std::string& fullpath/* = "" */)
{
    if (!_initialized) {

#ifdef USE_USER_DEFAULT_LS
        cocos2d::LocalStorageIos::getInstance();
        _initialized = 1;
#else
        int ret = 0;
		
        if (fullpath.empty())
            ret = sqlite3_open(":memory:", &_db);
        else
            ret = sqlite3_open(fullpath.c_str(), &_db);

        localStorageCreateTable();

        // SELECT
        const char *sql_select = "SELECT value FROM data WHERE key=?;";
        ret |= sqlite3_prepare_v2(_db, sql_select, -1, &_stmt_select, nullptr);

        // REPLACE
        const char *sql_update = "REPLACE INTO data (key, value) VALUES (?,?);";
        ret |= sqlite3_prepare_v2(_db, sql_update, -1, &_stmt_update, nullptr);

        // DELETE
        const char *sql_remove = "DELETE FROM data WHERE key=?;";
        ret |= sqlite3_prepare_v2(_db, sql_remove, -1, &_stmt_remove, nullptr);
        
        // Clear
        const char *sql_clear = "DELETE FROM data;";
        ret |= sqlite3_prepare_v2(_db, sql_clear, -1, &_stmt_clear, nullptr);

        if (ret != SQLITE_OK) {
            printf("Error initializing DB\n");
            // report error
        }
		
        _initialized = 1;
#endif
    }
}

void localStorageFree()
{
    if (_initialized) {
#ifdef USE_USER_DEFAULT_LS
        cocos2d::LocalStorageIos::destroyInstance();
#else
        sqlite3_finalize(_stmt_select);
        sqlite3_finalize(_stmt_remove);
        sqlite3_finalize(_stmt_update);

        sqlite3_close(_db);
#endif
        _initialized = 0;
    }
}

/** sets an item in the LS */
void localStorageSetItem( const std::string& key, const std::string& value)
{
    assert( _initialized );
#ifdef USE_USER_DEFAULT_LS
    cocos2d::LocalStorageIos::getInstance()->setItem(key, value);
#else
    int ok = sqlite3_bind_text(_stmt_update, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    ok |= sqlite3_bind_text(_stmt_update, 2, value.c_str(), -1, SQLITE_TRANSIENT);

    ok |= sqlite3_step(_stmt_update);
	
    ok |= sqlite3_reset(_stmt_update);
	
    if (ok != SQLITE_OK && ok != SQLITE_DONE)
        printf("Error in localStorage.setItem()\n");
#endif
}

/** gets an item from the LS */
bool localStorageGetItem( const std::string& key, std::string *outItem )
{
    assert( _initialized );

#ifdef USE_USER_DEFAULT_LS
    bool ret = cocos2d::LocalStorageIos::getInstance()->getItem(key, outItem);
    return ret;
#else
    int ok = sqlite3_reset(_stmt_select);

    ok |= sqlite3_bind_text(_stmt_select, 1, key.c_str(), -1, SQLITE_TRANSIENT);
    ok |= sqlite3_step(_stmt_select);
    const unsigned char *text = sqlite3_column_text(_stmt_select, 0);

    if (ok != SQLITE_OK && ok != SQLITE_DONE && ok != SQLITE_ROW)
    {
        printf("Error in localStorage.getItem()\n");
        return false;
    }
    else if (!text)
    {
        return false;
    }
    else
    {
        outItem->assign((const char*)text);
        return true;
    }
#endif
}

/** removes an item from the LS */
void localStorageRemoveItem( const std::string& key )
{
    assert( _initialized );

#ifdef USE_USER_DEFAULT_LS
    cocos2d::LocalStorageIos::getInstance()->removeItem(key);
#else
    int ok = sqlite3_bind_text(_stmt_remove, 1, key.c_str(), -1, SQLITE_TRANSIENT);
	
    ok |= sqlite3_step(_stmt_remove);
	
    ok |= sqlite3_reset(_stmt_remove);

    if (ok != SQLITE_OK && ok != SQLITE_DONE)
        printf("Error in localStorage.removeItem()\n");
#endif
}

/** removes all items from the LS */
void localStorageClear()
{
    assert( _initialized );
#ifdef USE_USER_DEFAULT_LS
    cocos2d::LocalStorageIos::getInstance()->clear();
#else
    int ok = sqlite3_step(_stmt_clear);
    
    if( ok != SQLITE_OK && ok != SQLITE_DONE)
        printf("Error in localStorage.clear()\n");
#endif
}

#endif // #if (CC_TARGET_PLATFORM != CC_PLATFORM_ANDROID)
