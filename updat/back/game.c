#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

#include "gf3d_entity.h"
#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_cube.h"
#include "gf3d_sprite.h"

void game_think(struct Entity_S* self){
	//nothing yet
}
void game_update(struct Entity_S* self,Matrix4 modelMat2){
	gfc_matrix_make_translation(modelMat2, self->position);
	gfc_matrix_rotate(modelMat2, modelMat2, (self->rotation.x + 90)* GFC_DEGTORAD, vector3d(0, 0, 1));
}
void game_touch(struct Entity_S* self){
	//nothing yet
}

int main(int argc, char *argv[])
{
	int done = 0;
	int a;
	int i = 0;
	Uint8 validate = 0;
	const Uint8 * keys;
	Uint32 bufferFrame = 0;
	VkCommandBuffer commandBuffer;
	Model *model;
	Matrix4 modelMat;
	Model *model2;
	Matrix4 modelMat2;
	Sprite *mouse = NULL;
	int mousex, mousey;
	Uint32 mouseFrame = 0;
	float accelForward = 0.0;
	struct Entity_S *ent;
	Entity *building;
	Entity *car;
	float damage = 0;
	Entity *powerup;

	for (a = 1; a < argc; a++)
	{
		if (strcmp(argv[a], "-disable_validate") == 0)
		{
			validate = 0;
		}
	}

	init_logger("gf3d.log");
	slog("gf3d begin");
	gf3d_vgraphics_init(
		"gf3d",                 //program name
		1920,                   //screen width
		1080,                    //screen height
		vector4d(0.51, 0.75, 1, 1),//background color
		0,                      //fullscreen
		validate                //validation
		);

	// main game loop
	slog("gf3d main loop begin");
	model = gf3d_model_load("building");
	model2 = gf3d_model_load("APC");
	gfc_matrix_identity(modelMat);
	gfc_matrix_make_translation(
		modelMat,
		vector3d(10, -15, -1));
	gfc_matrix_rotate(
		modelMat,
		modelMat,
		0.4,
		vector3d(0, 0, 1));
	gfc_matrix_identity(modelMat2);
	gfc_matrix_make_translation(
		modelMat2,
		vector3d(10, 0, 0));

	gf3d_entity_manager_init(10);
	powerup = gf3d_entity_new();
	
	car = gf3d_entity_new();
	car->rotation = vector3d(0, 0, 0);
	car->carNum = 1; 
	building = gf3d_entity_new();
	building->health = 2000;
	building->position = vector3d(10, -15, -1);
	car->box.boxext = vector3d(5, 5, 5);
	building->box.boxext = vector3d(40, 40, 40);
	powerup->position = vector3d(15, 15, 15);
	powerup->box.boxext = vector3d(5, 5, 5);
	//gf3d_set_entity_bounding_box(car);
	//gf3d_set_entity_bounding_box(building);
	mouse = gf3d_sprite_load("images/pointer.png", 32, 32, 16);
	while (!done)
	{
		SDL_PumpEvents();   // update SDL's internal event structures
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		//update game things here
		SDL_Event event;
		Vector3D forward;
		car->box.boxpos = car->position;
		building->box.boxpos = building->position;
		if (gf3d_colliding(car->box, building->box)&& damage != 1){
			slog("it worked.");
			model2 = gf3d_model_load("APCDAM");
			damage = 1;

		}
		if (gf3d_colliding(car->box, powerup->box)){
			float decider = gfc_random();
			if (decider >= 0.6){
				powerup->poweruppickup = 3;
			}
			if (decider < 0.6 && decider >= 0.3){
				powerup->poweruppickup = 2;
				powerup->poweruptime = 3500;
			}
			if (decider < 0.3){
				powerup->poweruppickup = 1;
				powerup->poweruptime = 6000;
			}
		}
		if (keys[SDL_SCANCODE_1]){
			car->carNum = 1;
			model2 = gf3d_model_load("APC");
			if (car->carNum == 1){
				car->health = 100;
				car->accel = 0.00005;
				car->maxspeed = 0.05;
				car->grip = 0.000025;
				car->armor = 10;
				car->handling = 0.2;
				car->guntype = 1;
			}
		}
		if (keys[SDL_SCANCODE_2]){
			car->carNum = 2;
			model2 = gf3d_model_load("thump");
			if (car->carNum == 2){
				car->health = 50;
				car->accel = 0.0001;
				car->maxspeed = 0.09;
				car->grip = 0.00005;
				car->armor = 5;
				car->handling = 0.4;
				car->guntype = 2;
			}
		}
		if (keys[SDL_SCANCODE_3]){
			car->carNum = 3;
			model2 = gf3d_model_load("tank");
			if (car->carNum == 3){
				car->health = 150;
				car->accel = 0.00003;
				car->maxspeed = 0.03;
				car->grip = 0.00001;
				car->armor = 15;
				car->handling = 0.1;
				car->guntype = 3;
			}
		}
		if (car->accelForward > 0.000001){//'friction code goes here, have to change the float to a modifiable value in ent system
			car->accelForward -= car->grip;
		}
		if (keys[SDL_SCANCODE_UP]){
			if (car->accelForward <= car->maxspeed){
				car->accelForward += car->accel;
			}
			if (car->accelForward > car->maxspeed){
				car->accelForward -= 0.003;
			}
			// jiwa was here uwu 
				while (!(car->accelForward <= 0.000001)){
					i = 1;
					break;
				}
				//game_think(e);
				i = 0;
		}
		if (keys[SDL_SCANCODE_DOWN]){
			car->accelForward -= car->accel;
			if (car->accelForward < -0.015){
				car->accelForward += 0.006;
			}
		}
		if (keys[SDL_SCANCODE_P]){
			car->maxspeed = 100;
			car->accel = 0.01;
		}
		if (keys[SDL_SCANCODE_I]){
			model2 = gf3d_model_load("APCGOLD");
		}
		if (keys[SDL_SCANCODE_O]){
			model2 = gf3d_model_load("APC");
		}
		if (keys[SDL_SCANCODE_SPACE]){
			if (car->guntype == 0){
				slog("click");
			}
			if (car->guntype == 1){
				//machineguncode

			}
			if (car->guntype == 2){
				//shotguncode
			}
			if (car->guntype == 3){
				//rocketcode
			}
		}
		//game_think(e);
		game_update(car, modelMat2);
		float handlingslow = car->handling / 2.5;
		if (i = 1){
			if (keys[SDL_SCANCODE_RIGHT] && (car->accelForward > 0.05 || car->carNum ==3)){
				car->rotation.x -= car->handling;
			}
			if (keys[SDL_SCANCODE_LEFT] && (car->accelForward > 0.05 || car->carNum == 3)){
				car->rotation.x += car->handling;
			}
			if (keys[SDL_SCANCODE_RIGHT] && car->accelForward < 0.05 && car->accelForward >0.00075){
				car->rotation.x -= (handlingslow);
			}
			if (keys[SDL_SCANCODE_LEFT] && car->accelForward < 0.05 && car->accelForward >0.00075){
				car->rotation.x += (handlingslow);
			}
			vector3d_angle_vectors(car->rotation, &forward, NULL, NULL);
			vector3d_set_magnitude(&forward, car->accelForward);
			vector3d_add(car->position, car->position, forward);
			gfc_matrix_make_translation(modelMat2, car->position);
			game_update(car, modelMat2);
		}
	// configure render command for graphics command pool
	// for each mesh, get a command and configure it from the pool
	bufferFrame = gf3d_vgraphics_render_begin();
	gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_pipeline(), bufferFrame);
	commandBuffer = gf3d_command_rendering_begin(bufferFrame);

	gf3d_model_draw(model2, bufferFrame, commandBuffer, modelMat2,0);
	gf3d_model_draw(model, bufferFrame, commandBuffer, modelMat,0);
	gf3d_command_rendering_end(commandBuffer);

	gf3d_vgraphics_render_end(bufferFrame);

	if (keys[SDL_SCANCODE_ESCAPE])done = 1; // exit condition
}
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
