#ifndef UNITTESTS_H
#define UNITTESTS_H

#include "../virtualmachines.h"
#include "../xdflhandler.h"

void testNPaths(VMPrototype *vm_);
void testScripting(VMPrototype *vm_);
void testVoxels(VMPrototype *vm);
void testXDFLPaths(VMPrototype *vm);
void testXDFLParsing(VMPrototype *vm);

void testNewVM(VMPrototype* vm);
void testNewAIWriteTest(ArduinoInterface* ai);



#endif // UNITTESTS_H
