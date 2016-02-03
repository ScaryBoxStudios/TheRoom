#include "UUID.hpp"
#ifdef _WIN32
    #include <windows.h>
    #include <rpc.h>
    #include <wincrypt.h>
#else
    #include <uuid/uuid.h>
#endif

rUUID::rUUID()
  : mUUID("")
{
    // Target specific UUID generation
#ifdef _WIN32
    ::UUID wUUID = {};

    // Pass mWinUUID struct to be filled in UuidCreate function
    RPC_STATUS rVal = UuidCreate(&wUUID);

    if (rVal == RPC_S_OK)
    {
        // All ok
    }
    else if (rVal == RPC_S_UUID_LOCAL_ONLY)
    {
        // The UUID is guaranteed to be unique to this computer only.
    }
    else if (rVal == RPC_S_UUID_NO_ADDRESS)
    {
        // Cannot get Ethernet or token-ring hardware address for this computer.
    }

    // The dynamically allocated char* that will be returned from UuidToString
    char* temp = 0;

    // Takes the WinAPI Uuid object and a ptr to a char* and dynamically allocates and stores a c string with the UUID value
    UuidToString(&wUUID, (RPC_CSTR*)&temp);

    // Copy the dynamically allocated c string to our class member buffer
    mUUID = std::string(temp);

    // Frees the dynamically allocated c string
    RpcStringFree((RPC_CSTR*)&temp);

#else
    // Gen the uuid
    uuid_t uuid = {};
    uuid_generate_random (uuid);

    // Copy to holder
    char s[37];
    uuid_unparse(uuid, s);
    mUUID = std::string(s);
#endif
}

rUUID::rUUID(const rUUID& other)
  : mUUID(other.mUUID)
{
}

void rUUID::operator=(const rUUID& rhs)
{
    mUUID = rhs.mUUID;
}

bool rUUID::operator==(const rUUID& rhs) const
{
    return mUUID == rhs.mUUID;
}

bool rUUID::operator<(const rUUID& rhs) const
{
    return mUUID < rhs.mUUID;
}

const char* rUUID::c_str() const
{
    return mUUID.c_str();
}

rUUID::operator const char*() const
{
    return this->c_str();
}

bool StringToUUID(const char* str, rUUID& uuid)
{
#ifdef _WIN32
    ::UUID wUUID = {};
    RPC_STATUS rVal = UuidFromString((unsigned char*) str, &wUUID);
    if (rVal == RPC_S_INVALID_STRING_UUID)
        return false;

    char* temp = 0;
    UuidToString(&wUUID, (RPC_CSTR*)&temp);
    uuid.mUUID = std::string(temp);
    RpcStringFree((RPC_CSTR*)&temp);

    return true;
#else
    uuid_t lUUID = {};
    int rVal = uuid_parse(str, lUUID);
    if (rVal != 0)
        return false;

    char s[37];
    uuid_unparse (uuid, s);
    uuid.mUUID = std::string(s);
    return true;
#endif
}
