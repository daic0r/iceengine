//
//  ComponentIds.h
//  IceCraft
//
//  Created by Matthias Gruen on 03.10.20.
//  Copyright © 2020 Matthias Gruen. All rights reserved.
//

#ifndef ComponentIds_h
#define ComponentIds_h

#include <string>
#include <Entities/Entity.h>

namespace Ice {

enum ComponentIds : EntityComponentId {
    TERRAIN_COMPONENT,
    MESH_COMPONENT,
    TEXTURE_COMPONENT,
    RENDER_MATERIALS_COMPONENT,
    TRANSFORM_COMPONENT,
    MODEL_REFERENCE_COMPONENT,
    PROJECTION_COMPONENT,
    CAMERA_COMPONENT,
    TAG_COMPONENT,
    INPUT_RECEIVER_COMPONENT,
    SKYBOX_COMPONENT,
    SKELETON_COMPONENT,
    ANIMATED_MESH_COMPONENT,
    MODEL_ANIMATION_COMPONENT,
    GRAPH_NODE_COMPONENT,
    GRAPH_EDGE_COMPONENT,
    PATH_COMPONENT,
    PATH_SEGMENT_COMPONENT,
    PATH_ANIMATION_COMPONENT,
    LOCATION_COMPONENT,
    SAVE_COMPONENT,
    TOOLTIP_COMPONENT,
	BLUEPRINT_COMPONENT,
    MESH_GROUP_COMPONENT,
    LINKED_ENTITIES_COMPONENT,
    BUILDING_COMPONENT,
	CHARACTER_COMPONENT,
	VEHICLE_COMPONENT,
	INFECTION_COMPONENT,
	PARTICLE_SYSTEM_COMPONENT,
	MODEL_INSTANCE_TAG_COMPONENT,
	ANIMATED_MODEL_INSTANCE_TAG_COMPONENT,
	SUN_COMPONENT,
    WATER_TILE_COMPONENT,
    MAX_ID,
    SHARED_COMPONENT_BASE = 0x400
};

}

#endif /* ComponentIds_h */
