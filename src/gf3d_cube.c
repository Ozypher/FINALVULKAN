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
Entity *boolet;
Matrix4	modelmat1;

int gf3d_colliding(box a, box b){
	return (a.boxpos.x <= (b.boxpos.x+b.boxext.x) && (a.boxpos.x+a.boxext.x)>= b.boxpos.x &&
		a.boxpos.y <= (b.boxpos.y + b.boxext.y) && (a.boxpos.y+ a.boxext.y) >= b.boxpos.y&&
		a.boxpos.z <= (b.boxpos.z + b.boxext.z) && (a.boxpos.z + a.boxext.z) >= b.boxpos.z);
}
void bullet(Entity a, VkCommandBuffer commandBuffer, Uint32 bufferframe){
	Model *model1;
	model1 = gf3d_model_load("dino");
	boolet = gf3d_entity_new();
	gf3d_model_draw(model1, commandBuffer, bufferframe, modelmat1, 0);
	
}