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
#include <SDL_mixer.h>
#include "gfc_audio.h"

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
	float frame = 0;
	Model *model2;
	Matrix4 modelMat2;
	Sprite *mouse;
	Sprite *loss;
	Uint32 lossFrame = 0;
	int mousex, mousey;
	Uint32 mouseFrame = 0;
	float accelForward = 0.0;
	struct Entity_S *ent;
	Entity *building;
	Entity *building2;
	Entity *car;
	Entity *level1;
	float damage = 0;
	Matrix4 point;
	Entity *pointent;
	Model *pointmod;
	Entity *powerup;
	Matrix4 modelMat3;
	Model *model3;
	float degofradcam = 0;
	float change = 0;
	int lossCond = 0;
	int levelchoice = 1;
	Vector3D startingpos = vector3d(10, 0, 0);
	Sprite *scorecount;
	Uint32 scorenumber = 0;
	Sprite *scorenum;
	int stagenum = 0;
	int transition = 0;
	Sprite *menusys;
	Uint32 menuframe = 1;
	int menuopen;
	Sprite *levelnum;
	Sprite *youwin;
	//The music that will be played
	Mix_Music *gMusic = NULL;

	//The sound effects that will be used
	Mix_Chunk *gScratch = NULL;
	Mix_Chunk *gHigh = NULL;
	Mix_Chunk *gMedium = NULL;
	Mix_Chunk *gLow = NULL;

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
		"Night Drive",                 //program name
		1200,                   //screen width
		700,                    //screen height
		vector4d(1, 0, 0.69, 1),//background color
		0,                      //fullscreen
		validate                //validation
		);

	// main game loop
	mouse = gf3d_sprite_load("images/pointer.png", 32, 32, 16);
	loss = gf3d_sprite_load("images/youlose.png", 360, 360, 1);
	scorecount = gf3d_sprite_load("images/scorecount.png", 128, 128, 1);
	scorenum = gf3d_sprite_load("images/scorenum.png", 128, 128, 11);
	menusys = gf3d_sprite_load("images/menu.png", 360, 360, 1);
	levelnum = gf3d_sprite_load("images/levelcount.png", 128, 128, 2);
	youwin = gf3d_sprite_load("images/youwin.png", 360, 360, 1);
	slog("gf3d main loop begin");
	model2 = gf3d_model_load_animated("APC_anim", 1, 2);
	model = gf3d_model_load_animated("building_anim", 1, 2);
	model3 = gf3d_model_load_animated("building_anim", 1, 2);
	pointmod = gf3d_model_load_animated("tank_anim", 1, 2);
	gMusic = Mix_LoadMUS("main.mp3");
	gfc_audio_init(5, 2, 2, 2, 1, 1);
	if (gMusic == NULL)
	{
		slog("Failed to load beat music! SDL_mixer Error: %s\n", Mix_GetError());
	}
	gfc_matrix_identity(modelMat);
	gfc_matrix_identity(modelMat2);
	gfc_matrix_identity(modelMat3);
	gfc_matrix_identity(point);
	gfc_matrix_make_translation(
		modelMat2,
		startingpos);
	gfc_matrix_make_translation(
		point,
		vector3d(20, 0, 0));

	gf3d_entity_manager_init(20);
	pointent = gf3d_entity_new();
	pointent->position = vector3d(20, 0, 0);
	slog("ent made");
	powerup = gf3d_entity_new();
	level1 = gf3d_entity_new();
	car = gf3d_entity_new();
	car->rotation = vector3d(0, 0, 0);
	car->carNum = 1;
	//vulkan is stupiddddddd
	building2 = gf3d_entity_new();
	building = gf3d_entity_new();
	building->health = 2000;
	pointent->box.boxext = vector3d(3, 3, 3);
	car->box.boxext = vector3d(5, 5, 5);
	building->box.boxext = vector3d(20, 10, 20);
	building2->box.boxext = vector3d(20, 10, 20);
	//gf3d_set_entity_bounding_box(car);
	float currota = 0;
	//gf3d_set_entity_bounding_box(building);
	SDL_Init(SDL_INIT_AUDIO);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	Mix_PlayMusic(gMusic, -1);
	while (!done)
	{
		float change = car->handling*0.0025;
		SDL_PumpEvents();   // update SDL's internal event structures
		keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
		SDL_GetMouseState(&mousex, &mousey);
		//vector3d_add(car->camposition, car->position, vector3d(-10, 0, 4));
		//vector3d_rotate_about_z(&car->camposition, currota);
		vector3d_copy(car->camposition, vector3d(-15, 0, 0));
		vector3d_rotate_about_z(&car->camposition, currota);
		vector3d_add(car->camposition, car->camposition, car->position);
		car->camposition.z += 4;
		gf3d_camera_look_at(car->camposition, car->position, vector3d(0, 0, 1));
		if (stagenum == 0 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(20, 0, 0));
			pointent->position = vector3d(20, 0, 0);
		}
		if (stagenum == 1 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(40, 0, 0));
			pointent->position = vector3d(40, 0, 0);
		}
		if (stagenum == 2 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(50, -15, 0));
			pointent->position = vector3d(50, -15, 0);
		}
		if (stagenum == 3 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(40, -35, 0));
			pointent->position = vector3d(40, -35, 0);
		}
		if (stagenum == 4 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(20, -45, 0));
			pointent->position = vector3d(20, -45, 0);
		}
		if (stagenum == 5 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(0, -35, 0));
			pointent->position = vector3d(0, -35, 0);
		}
		if (stagenum == 6 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(-10, -15, 0));
			pointent->position = vector3d(-10, -15, 0);
		}
		if (stagenum == 7 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(10, 0, 0));
			pointent->position = vector3d(10, 0, 0);
		};
		if (stagenum == 8 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(30, 0, 0));
			pointent->position = vector3d(30, 0, 0);
		};
		if (stagenum == 9 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(50, 0, 0));
			pointent->position = vector3d(50, 0, 0);
		};
		if (stagenum == 10 && levelchoice == 1){
			gfc_matrix_make_translation(
				point,
				vector3d(70, 0, 0));
			pointent->position = vector3d(70, 0, 0);
		};
		if (scorenumber == 10 && levelchoice == 1){
			levelchoice = 2;
			if (transition = 0){
				car->position = startingpos;
				transition = 1;
			};
			scorenumber = 0;
			stagenum = 0;
		};
		if (stagenum == 0 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(10, -25, 0));
			pointent->position = vector3d(10, -25, 0);
		}
		if (stagenum == 1 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(-10, -25, 0));
			pointent->position = vector3d(-10, -25, 0);
		}
		if (stagenum == 2 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(-25, -15, 0));
			pointent->position = vector3d(-25, -15, 0);
		}
		if (stagenum == 3 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(0, 0, 0));
			pointent->position = vector3d(0, 0, 0);
		}
		if (stagenum == 4 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(30, 0, 0));
			pointent->position = vector3d(30, 0, 0);
		}
		if (stagenum == 5 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(35,-25, 0));
			pointent->position = vector3d(35, -25, 0);
		}
		if (stagenum == 6 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(0, -25, 0));
			pointent->position = vector3d(0, -25, 0);
		}
		if (stagenum == 7 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(-20, -35, 0));
			pointent->position = vector3d(-20, -35, 0);
		}
		if (stagenum == 8 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(0, -55, 0));
			pointent->position = vector3d(0, -55, 0);
		}
		if (stagenum == 9 && levelchoice == 2){
			gfc_matrix_make_translation(
				point,
				vector3d(30, -45, 0));
			pointent->position = vector3d(30, -45, 0);
		}
		if (levelchoice == 2 && scorenumber == 10){
			levelchoice = 3;
		};
		if (levelchoice == 1){
			gfc_matrix_make_translation(
				modelMat,
				vector3d(10, -25, -1));
			building->position = vector3d(10, -25, -1);
			gfc_matrix_make_translation(
				modelMat3,
				vector3d(30, -15, -1));
			building2->position = vector3d(30, -15, -1);
		};
		if (levelchoice == 2){
			gfc_matrix_make_translation(
				modelMat,
				vector3d(10, -15, -1));
			building->position = vector3d(10, -15, -1);
			gfc_matrix_make_translation(
				modelMat3,
				vector3d(10, -40, -1));
			building2->position = vector3d(10, -40, -1);
		};
		//update game things here
		SDL_Event event;
		Vector3D forward;
		car->box.boxpos = car->position;
		pointent->box.boxpos = pointent->position;
		building->box.boxpos = vector3d(building->position.x - 5, building->position.y, building->position.z);
		building2->box.boxpos = vector3d(building2->position.x - 5, building2->position.y, building2->position.z);
		if (gf3d_colliding(car->box, building->box) || gf3d_colliding(car->box,building2->box)){
			model2 = gf3d_model_load_animated("APCDAM_anim",1,2);
			car->accelForward = 0;
			lossCond = 1;
		};
		while (gf3d_colliding(car->box, pointent->box)){
			pointent->position = vector3d(300, 300, 300);
			stagenum += 1;
			scorenumber += 1;
			break;
		};
		if (keys[SDL_SCANCODE_1]){
			car->carNum = 1;
			model2 = gf3d_model_load_animated("APC_anim",1,2);
			if (car->carNum == 1){
				car->health = 100;
				car->accel = 0.00005;
				car->maxspeed = 0.05;
				car->grip = 0.000025;
				car->armor = 10;
				car->handling = 0.2;
				car->guntype = 1;
			}
		};
		if (keys[SDL_SCANCODE_2]){
			car->carNum = 2;
			model2 = gf3d_model_load_animated("thump_anim",1,2);
			if (car->carNum == 2){
				car->health = 50;
				car->accel = 0.0001;
				car->maxspeed = 0.09;
				car->grip = 0.00005;
				car->armor = 5;
				car->handling = 0.4;
				car->guntype = 2;
			}
		};
		if (keys[SDL_SCANCODE_3]){
			car->carNum = 3;
			model2 = gf3d_model_load_animated("tank_anim",1,2);
			if (car->carNum == 3){
				car->health = 150;
				car->accel = 0.00003;
				car->maxspeed = 0.03;
				car->grip = 0.00001;
				car->armor = 15;
				car->handling = 0.1;
				car->guntype = 3;
			}
		};
		if (keys[SDL_SCANCODE_R]){
			car->position = startingpos;
			car->rotation = vector3d(0, 0, 0);
			model2 = gf3d_model_load_animated("thump_anim", 1, 2);
			lossCond = 0;
			scorenumber = 0;
			stagenum = 0;
		};
		if (car->accelForward > 0.000001){//'friction code goes here, have to change the float to a modifiable value in ent system
			car->accelForward -= car->grip;
		};
		if (keys[SDL_SCANCODE_UP]){
			if (car->accelForward <= car->maxspeed){
				car->accelForward += car->accel;
			}
			if (car->accelForward > car->maxspeed){
				car->accelForward -= 0.003;
			}
				while (!(car->accelForward <= 0.000001)){
					i = 1;
					break;
				}
				//game_think(e);
				i = 0;
		};
		if (keys[SDL_SCANCODE_DOWN]){
			car->accelForward -= car->accel;
			if (car->accelForward < -0.015){
				car->accelForward += 0.006;
			}
		};
		if (keys[SDL_SCANCODE_P]){
			car->maxspeed = 100;
			car->accel = 0.01;
		};
		if (keys[SDL_SCANCODE_L]){
			levelchoice = 2;
		};
		if (keys[SDL_SCANCODE_K]){
			levelchoice = 1;
		};
		//game_think(e);
		game_update(car, modelMat2);
		float handlingslow = car->handling / 2.5;
		if (i = 1){
			if (keys[SDL_SCANCODE_RIGHT] && (car->accelForward > 0.05 || car->carNum ==3)){
				car->rotation.x -= car->handling;
				degofradcam += change;
				currota = car->rotation.x;
			}
			if (keys[SDL_SCANCODE_LEFT] && (car->accelForward > 0.05 || car->carNum == 3)){
				car->rotation.x += car->handling;
				degofradcam -= change;
				currota = car->rotation.x;
			}
			if (keys[SDL_SCANCODE_RIGHT] && car->accelForward < 0.05 && car->accelForward >0.00075){
				car->rotation.x -= (handlingslow);
				degofradcam += change;
				currota = car->rotation.x;
			}
			if (keys[SDL_SCANCODE_LEFT] && car->accelForward < 0.05 && car->accelForward >0.00075){
				car->rotation.x += (handlingslow);
				degofradcam -= change;
				currota = car->rotation.x;
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
	gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_model_pipeline(), bufferFrame);
	gf3d_pipeline_reset_frame(gf3d_vgraphics_get_graphics_overlay_pipeline(), bufferFrame);
	frame = frame + 0.05;
	if (frame >= 1)frame = 0;
	mouseFrame = (mouseFrame + 1) % 16;
	lossFrame = 1;

	commandBuffer = gf3d_command_rendering_begin(bufferFrame, gf3d_vgraphics_get_graphics_model_pipeline());

	gf3d_model_draw(model2, bufferFrame, commandBuffer, modelMat2,(Uint32)frame);
	gf3d_model_draw(model, bufferFrame, commandBuffer, modelMat, (Uint32)frame);
	gf3d_model_draw(model3, bufferFrame, commandBuffer, modelMat3,(Uint32)frame);
	gf3d_model_draw(pointmod, bufferFrame, commandBuffer, point, (Uint32)frame);
	gf3d_command_rendering_end(commandBuffer);

	commandBuffer = gf3d_command_rendering_begin(bufferFrame, gf3d_vgraphics_get_graphics_overlay_pipeline());

	gf3d_sprite_draw(scorecount, vector2d(0,0), lossFrame, bufferFrame, commandBuffer);
	gf3d_sprite_draw(scorenum, vector2d(50, 0), scorenumber, bufferFrame, commandBuffer);
	gf3d_sprite_draw(levelnum, vector2d(1142, 0), levelchoice - 1, bufferFrame, commandBuffer);
	if (levelchoice == 3){
		gf3d_sprite_draw(youwin, vector2d(500, 270), lossFrame, bufferFrame, commandBuffer);
	}
	if (lossCond == 1){
		gf3d_sprite_draw(loss, vector2d(500, 270), lossFrame, bufferFrame, commandBuffer);
	}
	if (keys[SDL_SCANCODE_ESCAPE]){
		menuopen = 1;
	}
	if (menuopen == 1){
		gf3d_sprite_draw(menusys, vector2d(500, 270), menuframe, bufferFrame, commandBuffer);
		gf3d_sprite_draw(mouse, vector2d(mousex, mousey), mouseFrame, bufferFrame, commandBuffer);
		while (SDL_PollEvent(&event)) // check to see if an event has happened
		{
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				if ((mousex >= 500 && mousex <= 680) && (mousey >= 270 && mousey <= 360)){
					menuopen = 0;
				}
				if ((mousex >= 500 && mousex <= 680) && (mousey >= 360 && mousey <= 450)){
					done = 1;
				}
			}
		}
	}


	gf3d_command_rendering_end(commandBuffer);

	gf3d_vgraphics_render_end(bufferFrame);
}
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
