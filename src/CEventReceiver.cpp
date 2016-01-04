// Include headers
#include "include/CEventReceiver.h"

// Constructor
CEventReceiver::CEventReceiver()
{
	// Set the keypress array to false
	for (int i = 0; i < KEY_KEY_CODES_COUNT; i++)
	{
	  keys[i] = false;
	}
}

// Event handler function
bool CEventReceiver::OnEvent(SEvent Event)
{
   if (Event.EventType == EET_KEY_INPUT_EVENT)
   {
      keys[Event.KeyInput.Key] = Event.KeyInput.PressedDown;
   }

   return false;
}

// Get key state
bool CEventReceiver::getKeyState(EKEY_CODE key)
{
   return keys[key];
}
