//
// Created by idezu on 17/01/2026.
//

#include <fast_obj.h>
#include "model.h"

#include <assert.h>
#include <stdlib.h>

#include <debug.h>
#include <string.h>

#include "graphicspipeline.h"

VkResult loadModel(Vertex** vertices, uint32_t* vertexBufferSize, uint32_t** indices, uint32_t* indexBufferSize) {
    fastObjMesh* mesh =  fast_obj_read("obj/viking_room.obj");

    if (mesh == NULL) {
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    *vertexBufferSize = mesh->position_count * sizeof(Vertex);
    *vertices = malloc(*vertexBufferSize);
    assert(*vertices != NULL);

    int32_t* uniqueVertices = calloc(mesh->position_count,sizeof(int32_t));
    assert(uniqueVertices != NULL);
    memset(uniqueVertices,-1,mesh->position_count*sizeof(int32_t));

    *indexBufferSize = mesh->index_count * sizeof(uint32_t);
    *indices = malloc(*indexBufferSize);
    assert(*indices != NULL);

    uint32_t vertex_length = 0;
    uint32_t index_length = 0;

    for (uint32_t k = 0; k < mesh->group_count; k++) {
        uint32_t start = mesh->groups[k].index_offset;
        uint32_t end   = start + mesh->groups[k].face_count * 3;
        for (uint32_t i = start; i < end; i++) {
            fastObjIndex currIndex = mesh->indices[i];

            if (uniqueVertices[currIndex.p-1] == -1) { //TODO: make it an hash-based vertex deduplication instead of a index based one
                Vertex vertex = {
                    .pos = {
                        mesh->positions[3*(currIndex.p) + 0],
                        mesh->positions[3*(currIndex.p) + 1],
                        mesh->positions[3*(currIndex.p) + 2],
                    },
                    .texCoord = {
                        mesh->texcoords[2*(currIndex.t) + 0],
                        1.0f - mesh->texcoords[2*(currIndex.t) + 1],
                    },
                    .color = {
                        1.0f,1.0f,1.0f
                    },
                };
                (*vertices)[vertex_length] = vertex;
                uniqueVertices[currIndex.p-1] = vertex_length;
                vertex_length++;
            }
            (*indices)[index_length] = uniqueVertices[currIndex.p-1];
            index_length++;
        }
    }

    debug("vertices count is %d and its size is %d\n",vertex_length,*vertexBufferSize);
    debug("indices count is %d and its size is %d\n",index_length,*indexBufferSize);

    fast_obj_destroy(mesh);

    return VK_SUCCESS;
}
