
#include "Include/FTSDKFactory.h"

namespace com::ft::sdk
{
    std::unique_ptr<FTSDK> FTSDKFactory::get(const std::string& configJson)
    {
        //return std::make_unique<FTSDK>(configJson);
        return std::unique_ptr<FTSDK>(new FTSDK(configJson));
    }
}