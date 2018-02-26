#include <stdlib.h>
unsigned int sceLibcHeapExtendedAlloc = 1;			/* Switch to dynamic allocation */
size_t       sceLibcHeapSize = 256 * 1024 * 1024;	/* Set up heap area upper limit as 256 MiB */


//#include "ExampleRenderer.h"

#include "PS4Input.h"

#include <iostream>
#include "Ps4SoundSystem.h"

#include "MiniGraphicsPipeline.h"

int main(void) {
	//ExampleRenderer renderer;
	MiniGraphicsPipeline gp;

	PS4Input		input		= PS4Input();

	Ps4SoundSystem*	soundSystem = new Ps4SoundSystem(8);

	while (true) {
		input.Poll();
		//renderer.UpdateScene(0.02);
		//renderer.RenderScene();
		//renderer.SwapBuffers();

		gp.UpdateScene(0.02);
		gp.RenderScene();
	

		if (input.GetButton(0)) {
			std::cout << "LOL BUTTON" << std::endl;
		}

		if (input.GetButton(1)) {
			return 1;
		}
	}

	return 1;
}