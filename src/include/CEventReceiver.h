#ifndef __CEVENTRECEIVER_H
#define __CEVENTRECEIVER_H

// Include headers
#include "irr/irrlicht.h"

// Use namespaces
using namespace irr;
using namespace core;
using namespace video;
using namespace gui;
using namespace io;
using namespace scene;

// Event Receiver Class
class CEventReceiver : public IEventReceiver
{
   public:
      CEventReceiver();
      virtual bool OnEvent(SEvent Event);
      bool getKeyState(EKEY_CODE key);
   private:
      bool keys[KEY_KEY_CODES_COUNT];
};

#endif
