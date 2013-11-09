/*
 * Serializers.h
 *
 *  Created on: 2013/10/28
 *      Author: mugwort_rc
 */

#ifndef SERIALIZERS_H_
#define SERIALIZERS_H_

#include "ElementTree.h"

namespace markdown{

std::wstring to_html_string(Element& element);

std::wstring to_xhtml_string(Element& element);

} // end of namespace markdown

#endif // SERIALIZERS_H_
