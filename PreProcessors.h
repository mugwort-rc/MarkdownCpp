/*
 * PreProcessors.h
 *
 *  Created on: 2013/10/25
 *      Author: mugwort_rc
 */

#ifndef PREPROCESSORS_H_
#define PREPROCESSORS_H_

#include "Processor.h"

namespace markdown{

class Markdown;  //!< forward declaration

OrderedDictProcessors build_preprocessors(Markdown* md_instance);

} // end of namespace markdown

#endif /* PREPROCESSORS_H_ */
