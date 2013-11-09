/*
 * Extension.cpp
 *
 *  Created on: 2013/11/04
 *      Author: mugwort_rc
 */

#include "Extension.h"

namespace  markdown {

Extension::Extension(const Config& config) :
    config(config)
{}

Extension::~Extension()
{}

std::wstring Extension::getConfig(const std::wstring &key, const std::wstring &default_val) const
{
    if ( this->config.find(key) != this->config.end() ) {
        return this->config.at(key).at(0);
    }
    return default_val;
}

void Extension::setConfig(const std::wstring &key, const std::vector<std::wstring> &value)
{
    this->config[key] = value;
}

} // end of namespace markdown
