// Include headers
#include "include/FinalProject.h"

// Use namespaces
using namespace irr;
using namespace core;
using namespace video;
using namespace gui;
using namespace io;
using namespace scene;

// Main
int main()
{
	// Declare an instance of the event receiver
	CEventReceiver Receiver;

	// Create a new Irrlicht Device
	IrrlichtDevice* Device = createDevice(
		EDT_OPENGL,                   // Device type
		dimension2d<s32>(800, 600),   // Window dimensions
		16,                           // Color depth
		false,                        // Fullscreen
		false,                        // Stencil buffer
		true,                         // V-sync
		&Receiver);                   // Event receiver

	// Retrieve the driver, scene manager, etc
	IVideoDriver* Driver = Device->getVideoDriver();
	ISceneManager* SceneManager = Device->getSceneManager();
	IGUIEnvironment* GUIEnv = Device->getGUIEnvironment();

    // Initialize a random number generator
    TRandomMersenne RandomNum(time(0));

    // Declare arrays of scene nodes, triangle selectors, collision animators, and lights
    ISceneNode* WallNodes[NUM_NODES];
    ILightSceneNode* LightNodes[NUM_NODES];
    ITriangleSelector* Selectors[NUM_NODES];
    ISceneNodeAnimator* CollisionResponse[NUM_NODES];
    IBillboardSceneNode* ObstructionNodes[NUM_NODES][NUM_OBSTRUCTIONS];
    aabbox3d<f32> obstructionBoxes[NUM_NODES][NUM_OBSTRUCTIONS];
    ITexture* ObstructionTexture;

    // Set pointers to 0
    for (s32 i = 0; i < NUM_NODES; i++)
    {
		WallNodes[i] = 0;
		LightNodes[i] = 0;
		Selectors[i] = 0;
		CollisionResponse[i] = 0;

    	for (s32 j = 0; j < NUM_OBSTRUCTIONS; j++)
    	{
			ObstructionNodes[i][j] = 0;
    	}
    }

	// Set the camera
	ICameraSceneNode* Camera = SceneManager->addCameraSceneNodeFPS(0, 100, 50);
	Camera->setPosition(vector3df(-30, 0, 0));
	Camera->setTarget(vector3df(0, 0, 0));

    // Get the wall and obstruction mesh
    IAnimatedMesh* WallMesh = SceneManager->getMesh("media/wall.3ds");

    // Check to see if the mesh was loaded successfully
    if (!WallMesh)
    {
        Device->closeDevice();
    }

    // Initialize the scene nodes
    for (s32 i = 0; i < NUM_NODES; i++)
    {
        // Set the wall nodes
        WallNodes[i] = SceneManager->addOctTreeSceneNode(WallMesh);
        WallNodes[i]->getMaterial(0).DiffuseColor = SColor(255, 100, 100, 100);

        // Initialize the triangle selectors
        Selectors[i] = SceneManager->createOctTreeTriangleSelector(
            WallMesh->getMesh(0),   // Mesh
            WallNodes[i],           // Node
            128);                   // Min polys
        WallNodes[i]->setTriangleSelector(Selectors[i]);
        Selectors[i]->drop();

        // Set the collision response animators
        CollisionResponse[i] = SceneManager->createCollisionResponseAnimator(
            WallNodes[i]->getTriangleSelector(),	// Triangle selector
            Camera,                                 // Parent
            vector3df(3, 3, 3),               		// Bounds
            vector3df(0, 0, 0),                     // Gravity
            vector3df(0, 0, 0));                    // Displacement
        Camera->addAnimator(CollisionResponse[i]);
        CollisionResponse[i]->drop();
    }

    // Initialize the positions and add light nodes
    f32 distanceToMove = (WallNodes[0]->getBoundingBox().MaxEdge.X - WallNodes[0]->getBoundingBox().getCenter().X) * 2;
    for (s32 i = 0; i < NUM_NODES; i++)
    {
        // Set the position
        WallNodes[i]->setPosition(vector3df(
            WallNodes[i]->getPosition().X + (distanceToMove * i),   // X coord
            WallNodes[i]->getPosition().Y,                          // Y coord
            WallNodes[i]->getPosition().Z));                        // Z coord

        // Add a dynamic light
        LightNodes[i] = SceneManager->addLightSceneNode(
            WallNodes[i],                               // Parent
            WallNodes[i]->getBoundingBox().getCenter(), // Position
            SColor(255, 150, 150, 255),                 // Color
            12);                                        // Radius
    }

	// Set up the obstruction texture
	ObstructionTexture = Driver->getTexture("media/obstruction.png");

	// Initialize the obstructions
	for (s32 i = 0; i < NUM_NODES; i++)
	{
		for (s32 j = 0; j < NUM_OBSTRUCTIONS; j++)
		{
			// Initialize the nodes
			ObstructionNodes[i][j] = SceneManager->addBillboardSceneNode(
				WallNodes[i],											// Parent
				dimension2d<f32>(OBSTRUCTION_SIZE, OBSTRUCTION_SIZE),	// Dimensions
				vector3df(												// Position
				WallNodes[i]->getTransformedBoundingBox().getCenter().X + (f32)RandomNum.IRandom(-20, 20),	// X
				WallNodes[i]->getTransformedBoundingBox().getCenter().Y + (f32)RandomNum.IRandom(-7, 7),	// Y
				WallNodes[i]->getTransformedBoundingBox().getCenter().Z + (f32)RandomNum.IRandom(-7, 7)));	// Z

			// Set the textures
			ObstructionNodes[i][j]->setMaterialType(EMT_TRANSPARENT_ALPHA_CHANNEL_REF);
			ObstructionNodes[i][j]->setMaterialTexture(0, ObstructionTexture);
		}
	}

    // Set the mouse cursor to invisible
    Device->getCursorControl()->setVisible(false);

    // Set the current bounding box and position vars
    s32 currentBox = 0;
    s32 lastBox = 0;
    s32 highestXNode = 0;
    f32 highestXPos = WallNodes[0]->getTransformedBoundingBox().MaxEdge.X;

    // Initialize a timer
    ITimer* Timer = Device->getTimer();
    u32 startTime = Timer->getRealTime();
    u32 nextColorChange = startTime + 5000;

	// Main loop
	while (Device->run())
	{
		// Send an event to the event receiver, making the camera move forward on its own
		SEvent Event;
		Event.EventType = EET_KEY_INPUT_EVENT;
		Event.KeyInput.Key = KEY_UP;
		Event.KeyInput.PressedDown = true;
		Camera->OnEvent(Event);

		// Handle events
		if (Receiver.getKeyState(KEY_UP) || Receiver.getKeyState(KEY_KEY_W))
		{
			if (Camera->getNearValue() - 0.01f > 0.05f)
			{
				Camera->setNearValue(Camera->getNearValue() - 0.01f);
			}
		}
		if (!Receiver.getKeyState(KEY_UP) && !Receiver.getKeyState(KEY_KEY_W))
		{
			if (Camera->getNearValue() + 0.01f < 1.0f)
			{
				Camera->setNearValue(Camera->getNearValue() + 0.01f);
			}
		}
		if (Receiver.getKeyState(KEY_ESCAPE))
		{
			Device->closeDevice();
		}

		// Get the camera position
		vector3df cameraPos = Camera->getPosition();

		// See which bounding box the camera is in
		for (s32 i = 0; i < NUM_NODES; i++)
		{
            if (WallNodes[i]->getTransformedBoundingBox().isPointTotalInside(cameraPos))
            {
                lastBox = currentBox;
                currentBox = i;
            }
		}

		// If the camera is in the new bounding box
		if (currentBox != lastBox)
		{
            // Loop through and see which bounding box is the farthest away from the camera
            for (s32 i = 0; i < NUM_NODES; i++)
            {
                if (WallNodes[i]->getTransformedBoundingBox().MaxEdge.X >= highestXPos)
                {
                    highestXPos = WallNodes[i]->getTransformedBoundingBox().MaxEdge.X;
                    highestXNode = i;
                }
            }

            // Set the node at the rear to the front
            WallNodes[lastBox]->setPosition(vector3df(
                WallNodes[highestXNode]->getPosition().X + distanceToMove,
                WallNodes[lastBox]->getPosition().Y,
                WallNodes[lastBox]->getPosition().Z));
		}

		// Change the light colors if the required amount of time has passed
		if (Timer->getRealTime() >= nextColorChange)
		{
		    for (s32 i = 0; i < NUM_NODES; i++)
		    {
                // Change the light data to a random color
                SLight* lightData = &LightNodes[i]->getLightData();
                lightData->DiffuseColor.r = RandomNum.Random();
                lightData->DiffuseColor.g = RandomNum.Random();
                lightData->DiffuseColor.b = RandomNum.Random();
		    }

		    // Set the next color change
		    nextColorChange += 5000;
		}

        // Set the window caption
        wchar_t caption[100];
        swprintf(caption, 100, L"Final Program | FPS[%d] | Position[X: %f Y: %f Z: %f] Box[%d]", Driver->getFPS(), cameraPos.X, cameraPos.Y, cameraPos.Z, currentBox);
        Device->setWindowCaption(caption);

		// Start the scene
		Driver->beginScene(
			true,
			true,
			SColor(255, 0, 0, 0));

		// Draw everything
		SceneManager->drawAll();
		GUIEnv->drawAll();

		// End the scene
		Driver->endScene();
	}

	Device->drop();

   return 0;
}
