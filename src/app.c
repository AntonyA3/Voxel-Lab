#include "../include/app.h"

void app_update_window_glfw(App *app, GLFWwindow *window){
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    app->windowRect.x = 0;
    app->windowRect.y = 0;
    app->windowRect.w= (float)width;
    app->windowRect.h = (float)height;
    app->windowRatio = width /(float)height;
}

void app_update_cursor_glfw(App *app, GLFWwindow *window){
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    app->cursor.dx = x - app->cursor.x;
    app->cursor.dy = y - app->cursor.y;
    app->cursor.x = x;
    app->cursor.y = y;
    app->cursor.clipX = (app->cursor.x / app->windowRect.w) * 2 - 1;
    app->cursor.clipY = -1 *((app->cursor.y / app->windowRect.h) * 2 - 1);
}

void app_update_cursor_ray(App *app){
    vec4 clipTarget = {app->appViewCursorClipX, -app->appViewCursorClipY, -1.0, 1.0};
    vec4 worldTarget;
    vec3 worldTarget3D, directionVector;
    memcpy(app->cursorRay.origin, app->cameraPosition, sizeof(vec3));

    mat4x4_mul_vec4(worldTarget, app->invertedMatrix, clipTarget);
    worldTarget3D[0] = worldTarget[0];
    worldTarget3D[1] = worldTarget[1];
    worldTarget3D[2] = worldTarget[2];
    vec3_sub(directionVector, worldTarget3D, app->cursorRay.origin);
    vec3_norm(app->cursorRay.direction, directionVector);
}


void app_use_pos_color_32_vertex(){
    glVertexAttribPointer(0, 3,GL_FLOAT,GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 4,GL_FLOAT,GL_FALSE, 7 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);
}

int app_generate_shader_program_from_file(App *app, const char *vertexShaderPath, const char *fragmentShaderPath){
    char* vertexText;
    char* fragmentText;
    char infoLog[1000];
    GLuint vertexShader, fragmentShader, program;
    GLint success;
    file_get_text(vertexShaderPath, &vertexText);
    file_get_text(fragmentShaderPath, &fragmentText);
    
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader,1, (const char**)&vertexText,NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(vertexShader, 1000, NULL,infoLog);
    if(!success){
        printf("vertex shader failed to load \n");
        printf("message: %s\n", infoLog);
    }
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const char**)&fragmentText, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    glGetShaderInfoLog(vertexShader, 1000, NULL,infoLog);
    if(!success){
        printf("fragment shader failed to load \n");
        printf("message: %s\n", infoLog);
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    glGetProgramInfoLog(program, 1000, NULL, infoLog);
    if(!success){
        printf("program failed to load \n");
        printf("message: %s\n", infoLog);
    }
    return program;
}

void app_ray_vs_y0(App *app, int *didHit, Ray ray, int *entity, float *bestDistance){
    if(ray.y > 0 && ray.dy < 0){
        float d = (-ray.y) / ray.dy;
        if((d < *bestDistance) && (d > 0)){
            *bestDistance = d;
            *didHit = 1;
            *entity = RAY_HIT_ENTITY_Y0;
        }
    } 
}

void app_ray_vs_plane(App *app, Plane plane, int *didHit, Ray ray, float *bestDistance){
    vec3 p0ml0;
    float l, denom, numer;
    vec3_sub(p0ml0, plane.point, ray.origin);
    numer = vec3_mul_inner(plane.normal, p0ml0);
    denom = vec3_mul_inner(plane.normal, ray.direction);
    l = numer / denom;
    if(l <= *bestDistance){
        *bestDistance = l;
        *didHit = 1;
    }
}

void app_generate_floor_grid_vertex_array(App *app){   
    int index = 0;
    app->floorGridVertexCount = grid_xz_get_vertex_count(app->floorGrid);
    int xStart = app->floorGrid.x;
    int zStart = app->floorGrid.z;
    int xEnd = app->floorGrid.x + app->floorGrid.xCount * app->floorGrid.sizeX;
    int zEnd = app->floorGrid.z + app->floorGrid.zCount * app->floorGrid.sizeZ;        
    app->floorGridVertexArray = calloc(app->floorGridVertexCount, sizeof(PosColor32Vertex));

    for(int i = 0; i <= app->floorGrid.xCount; i++){
        int x = app->floorGrid.x + i * app->floorGrid.sizeX;
        {
            float vertex[7] = {x, 0 , zStart, 1.0,1.0,1.0,1.0};
            memcpy(&app->floorGridVertexArray[index], vertex, sizeof(PosColor32Vertex));
        }
        index++;
        {
            float vertex[7] = {x, 0 , zEnd, 1.0,1.0,1.0,1.0};
            memcpy(&app->floorGridVertexArray[index], vertex, sizeof(PosColor32Vertex));
        }
        index++;
    }

    for(int i = 0; i <= app->floorGrid.zCount; i++){
        int z = app->floorGrid.z + i * app->floorGrid.sizeZ;

        {
            float vertex[7] = {xStart, 0 , z, 1.0,1.0,1.0,1.0};
            memcpy(&app->floorGridVertexArray[index], vertex, sizeof(PosColor32Vertex));
        }
        index++;
        {
            float vertex[7] = {xEnd, 0 , z, 1.0,1.0,1.0,1.0};
            memcpy(&app->floorGridVertexArray[index], vertex, sizeof(PosColor32Vertex));
        }
        index++;
    }
    glBufferData(GL_ARRAY_BUFFER, sizeof(PosColor32Vertex) * app->floorGridVertexCount, app->floorGridVertexArray, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

  
void app_update_voxel_head_model(App* app){
    /*
    PosColor32Vertex verticies[8];
    unsigned int elements[36];
    float size = (float)app->voxelModel.head->size/ 2;
    vec3 origin = {app->voxelModel.head->origin[0], 
        app->voxelModel.head->origin[1],
        app->voxelModel.head->origin[2]};
    vec3 halfExtents = {size, size, size};
    ColorRgbaF color = {{1.0,0.0,1.0,1.0}};
    
    box_generate_box_pos_color_32_vertex(origin, color, halfExtents, verticies, elements);
    glBindBuffer(GL_ARRAY_BUFFER, app->voxelHeadVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PosColor32Vertex) * 8, verticies, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->voxelHeadElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, elements, GL_DYNAMIC_DRAW);
    */
}

void app_update_ray_hit_model(App *app){
    PosColor32Vertex vertexArray[8]; 
    unsigned int elementArray[36];
    ColorRgbaF color = {{1.0,1.0,0.0,1.0}};
    vec3 size = {0.25, 0.25, 0.25};
    box_generate_box_pos_color_32_vertex(app->cursorRayHitPoint, color, size, vertexArray, elementArray);

    glBindBuffer(GL_ARRAY_BUFFER, app->vertexBufferId[VERTEX_BUFFER_ID_RAY_HIT]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PosColor32Vertex) * 8, vertexArray, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->elementBufferId[ELEMENT_BUFFER_ID_RAY_HIT]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, elementArray, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int app_get_cursor_button_next_state(int state, int isDown){
    int nextState;
    switch (state)
    {
    case CURSOR_BUTTON_STATE_PRESSED:
        nextState = CURSOR_BUTTON_STATE_DOWN* isDown;
        nextState += CURSOR_BUTTON_STATE_RELEASED * !isDown;
        break;
    case CURSOR_BUTTON_STATE_DOWN:
        nextState = CURSOR_BUTTON_STATE_DOWN * isDown;
        nextState += CURSOR_BUTTON_STATE_RELEASED * !isDown;
        break;
    case CURSOR_BUTTON_STATE_RELEASED:
        nextState = CURSOR_BUTTON_STATE_PRESSED * isDown;
        nextState += CURSOR_BUTTON_STATE_UP * !isDown;
        break;
    case CURSOR_BUTTON_STATE_UP:
        nextState = CURSOR_BUTTON_STATE_PRESSED * isDown;
        nextState += CURSOR_BUTTON_STATE_UP * !isDown;
        break;
    }
    return nextState;
}

//void app_update_voxel_model(App *app, VoxelTree voxelTree){
    /*
    app->voxelModelVertexArray = (PosColor32Vertex*) realloc(app->voxelModelVertexArray, 8 * app->voxelModel.voxelCount * sizeof(PosColor32Vertex));
    app->voxelModelElementArray = (unsigned int*) realloc(app->voxelModelElementArray, 36 * app->voxelModel.voxelCount * sizeof(unsigned int));
    voxel_model_generate_model(app->voxelModel, app->voxelModelVertexArray, app->voxelModelElementArray);
    
    glBindBuffer(GL_ARRAY_BUFFER, app->voxelModelVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, app->voxelModel.voxelCount * 8 * sizeof(PosColor32Vertex), app->voxelModelVertexArray, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->voxelModelElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, app->voxelModel.voxelCount * 36 * sizeof(unsigned int), app->voxelModelElementArray, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);  
    */
//}

void app_point_at_ray_distance(Ray ray, float distance, vec3 point){
    point[0] = ray.x + ray.dx * distance;
    point[1] = ray.y + ray.dy * distance;
    point[2] = ray.z + ray.dz * distance;
}
int app_point_in_yz_plane(vec3 point,float z, float depth, float y, float height){
    return (point[1] >= y) && (point[1] < (y + height)) &&
        (point[2] >= z) && (point[2] < (z + depth));
}
int app_point_in_xy_plane(vec3 point,float x, float width, float y, float height){
    return (point[0] >= x) && (point[0] < (x + width)) &&
        (point[1] >= y) && (point[1] < (y + height));
}
int app_point_in_xz_plane(vec3 point,float x, float width, float z, float depth){
    return (point[0] >= x) && (point[0] < (x + width)) &&
        (point[2] >= z) && (point[2] < (z + depth));
}
void app_ray_vs_aabb(App *app, Aabb aabb, int *didHit, Ray ray, float *bestDistance, int *side){
    float d;
    vec3 point;
    if(ray.dx > 0){
        d = (aabb.x - ray.x) / ray.dx;
        app_point_at_ray_distance(ray, d, point);
        if((d < *bestDistance) && (d > 0)){
            if(app_point_in_yz_plane(point,aabb.z, aabb.d, aabb.y, aabb.h)){
                *didHit = 1;
                *side = AABB_SIDE_LEFT;
                *bestDistance = d;
            } 
        }  
    }else if(ray.dx < 0 ){
        d = (aabb.x + aabb.w - ray.x) / ray.dx;
        app_point_at_ray_distance(ray, d, point);
        if((d < *bestDistance && d > 0) ){
           if(app_point_in_yz_plane(point,aabb.z, aabb.d, aabb.y, aabb.h)){
                *didHit = 1;
                *side = AABB_SIDE_RIGHT;
                *bestDistance = d;
           } 
        }
    }
    if(ray.dy > 0){
        d = (aabb.y - ray.y) / ray.dy;
        app_point_at_ray_distance(ray, d, point);
        if((d < *bestDistance) && d > 0){
            if(app_point_in_xz_plane(point, aabb.x, aabb.w, aabb.z, aabb.d)){
                *didHit = 1;
                *side = AABB_SIDE_BOTTOM;
                *bestDistance = d; 
            }  
        }
    }else if(ray.dy < 0 ){
        d = (aabb.y + aabb.h - ray.y) / ray.dy;
        app_point_at_ray_distance(ray, d, point);
        if((d < *bestDistance) && d > 0){
            if(app_point_in_xz_plane(point, aabb.x, aabb.w, aabb.z, aabb.d)){
                *didHit = 1;
                *side = AABB_SIDE_TOP;
                *bestDistance = d; 
            }  
        }   
    }
    if(ray.dz > 0){
        d = (aabb.z - ray.z) / ray.dz;
        app_point_at_ray_distance(ray, d, point);
        if((d < *bestDistance) && d > 0){
            if(app_point_in_xy_plane(point, aabb.x, aabb.w, aabb.y, aabb.h)){
                *didHit = 1;
                *side = AABB_SIDE_FRONT;
                *bestDistance = d; 
            }    
        }
    }else if(ray.dz < 0 ){
        d = (aabb.z + aabb.d - ray.z) / ray.dz;
        app_point_at_ray_distance(ray, d, point);
        if((d < *bestDistance) && d > 0){
            if(app_point_in_xy_plane(point, aabb.x, aabb.w, aabb.y, aabb.h)){
                *didHit = 1;
                *side = AABB_SIDE_BACK;
                *bestDistance = d; 
            }
        }
    }
}

void app_ray_vs_voxel_tree(App *app, int *didHit, VoxelTree voxelTree, Ray ray, int *entity, float *bestDistance, int *side){
    if(voxelTree.head){
        app_ray_vs_voxel(app, didHit, voxelTree.head, ray, entity, bestDistance, side);
    }
}

void app_ray_vs_voxel(App *app,  int *didHit, Voxel *voxel, Ray ray, int *entity, float *bestDistance, int *side){
    Aabb voxBox = voxel_to_aabb(*voxel);
    if(voxel_is_leaf(voxel)){
        int tempDidHit, tempSide;
        float tempDistance = *bestDistance;
        
        tempDidHit = ray_vs_aabb(ray,voxBox, &tempDistance, &tempSide);
        if(tempDidHit && (tempDistance < *bestDistance) && (tempDistance > 0) ){
            *didHit = tempDidHit;
            *bestDistance = tempDistance;
            *side = tempSide;
            *entity = RAY_HIT_ENTITY_VOXEL_MODEL;
        }
    }else{
        int tempHit, tempSide;
        float tempDistance = *bestDistance;
        tempHit = ray_vs_aabb(ray,voxBox, &tempDistance, &tempSide);
        int contains = aabb_contains_point(voxBox, ray.x, ray.y, ray.z);
        if(contains || tempHit ){
            for(int i = 0; i < 8; i++){
                if(voxel->child[i] != NULL){
                    app_ray_vs_voxel(app, didHit, voxel->child[i], ray, entity, bestDistance, side);
                }
            }
        }
    }
}

void app_hit_target_from_voxel_model_hit(App *app, int side, float *cellX, float *cellY, float *cellZ){

    switch (side)
    {
    case AABB_SIDE_LEFT:
        *cellX = roundf(*cellX) - 1;
        break;
    case AABB_SIDE_RIGHT:
        *cellX = roundf(*cellX);
        break;
    case AABB_SIDE_BOTTOM:
        *cellY = roundf(*cellY) - 1;
        break;
    case AABB_SIDE_TOP:
        *cellY = roundf(*cellY);
        break;
    case AABB_SIDE_FRONT:
        *cellZ = roundf(*cellZ) - 1;
        break;
    case AABB_SIDE_BACK:
        *cellZ = roundf(*cellZ);
        break;
    }
}

void app_update_app_view(App *app){
    app->appViewRect.x = app->propertyWidth + app->propertySelectorWidth;
    app->appViewRect.y = app->menuBarHeight;
    app->appViewRect.w = app->windowRect.w - app->propertySelectorWidth - app->propertyWidth;
    app->appViewRect.h = app->windowRect.h - app->menuBarHeight - app->actionBarHeight;
    app->appViewRatio = app->appViewRect.w / app->appViewRect.h;
}

void app_update_app_view_frame(App *app){
    glBindFramebuffer(GL_FRAMEBUFFER, app->frameBufferId[FRAME_BUFFER_ID_APP_VIEW]);
    glBindTexture(GL_TEXTURE_2D, app->textureId[TEXTURE_ID_APP_VIEW_TEXTURE]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, app->appViewRect.w, app->appViewRect.h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, app->textureId[TEXTURE_ID_APP_VIEW_TEXTURE], 0);
    
    glBindTexture(GL_TEXTURE_2D, app->textureId[TEXTURE_ID_APP_VIEW_TEXTURE_DEPTH]);

    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8,  app->appViewRect.w, app->appViewRect.h, 0,
        GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
    );
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, app->textureId[TEXTURE_ID_APP_VIEW_TEXTURE_DEPTH], 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

int app_voxel_edit_mode_add(App *app){
    /*
    float cellX = -1.0;
    float cellY = -1.0;
    float cellZ = -1.0;
    switch (app->cursorHitEntity)
    {
    case RAY_HIT_ENTITY_Y0:
        cellX = app->cursorRayHitPoint[0];
        cellY = 0.0f;
        cellZ = app->cursorRayHitPoint[2];
        break;
    case RAY_HIT_ENTITY_VOXEL_MODEL:
        cellX = app->cursorRayHitPoint[0];
        cellY = app->cursorRayHitPoint[1];
        cellZ = app->cursorRayHitPoint[2];
        app_hit_target_from_voxel_model_hit(&app, app->cursorSideHit, &cellX, &cellY, &cellZ);
        break;
    }

    switch (app->voxelEditorShapeId)
    {
    case VOXEL_EDITOR_SHAPE_ID_POINT:
        if(cellX >= 0 && cellY >= 0 && cellZ >= 0){
            voxel_model_add_voxel(&app->voxelModel, (unsigned int)cellX, (unsigned int)cellY, (unsigned int)cellZ);
            return 1;
        }
        break;
    case VOXEL_EDITOR_SHAPE_ID_CUBE:
        {
            Aabb aabb;
            aabb.x = cellX;
            aabb.y = cellY;
            aabb.z = cellZ;
            aabb.w = app->voxelEditAabbWidth;
            aabb.h = app->voxelEditAabbHeight;
            aabb.d = app->voxelEditAabbDepth;
            float maxX = aabb.x + aabb.w;
            float maxY = aabb.y + aabb.h;
            float maxZ = aabb.z + aabb.d;
            if(maxX >= 0.5 && maxY >= 0.5 && maxZ >= 0.5){
                voxel_model_add_voxel_from_aabb(&app->voxelModel, aabb);
                return 1;
            }
        }   
        break;
    case VOXEL_EDITOR_SHAPE_ID_SPHERE:
        {
            Sphere sphere;
            sphere.x = cellX + 0.5;
            sphere.y = cellY + 0.5;
            sphere.z = cellZ + 0.5;
            sphere.r = app->voxelEditSphereRadius;
            Aabb testBox = {0.5, 0.5, 0.5, 
                sphere.x + sphere.r + 0.5,
                sphere.y + sphere.r + 0.5, 
                sphere.z + sphere.r + 0.5
            };
            if(sphere_intersects_aabb(sphere, testBox)){
                voxel_model_add_voxel_from_sphere(&app->voxelModel, sphere);
                return 1;
            }
        }
        break;
    }
    return 0;
    */
   return 0;

}

int app_voxel_edit_mode_delete(App *app){
    return 0;
    /*
    float cellX = app->cursorRayHitPoint[0];
    float cellY = app->cursorRayHitPoint[1];
    float cellZ = app->cursorRayHitPoint[2];

    switch(app->cursorHitEntity){
        case RAY_HIT_ENTITY_Y0:
            break;
        case RAY_HIT_ENTITY_VOXEL_MODEL:
            {
                switch (app->cursorSideHit)
                {
                case AABB_SIDE_LEFT:
                    cellX = roundf(cellX);
                    break;
                case AABB_SIDE_RIGHT:
                    cellX = roundf(cellX) - 1;
                    break;
                case AABB_SIDE_BOTTOM:
                    cellY = roundf(cellY);
                    break;
                case AABB_SIDE_TOP:
                    cellY = roundf(cellY) - 1;
                    break;
                case AABB_SIDE_FRONT:
                    cellZ = roundf(cellZ);
                    break;
                case AABB_SIDE_BACK:
                    cellZ = roundf(cellZ) - 1;
                    break;
                }
            }
            break;
    }

    switch (app->voxelEditorShapeId)
    {
    case VOXEL_EDITOR_SHAPE_ID_POINT:
        if(app->cursorHitEntity == RAY_HIT_ENTITY_VOXEL_MODEL){     
            if(cellX >= 0 && cellY >= 0 && cellZ >= 0){
                cellX = floorf(cellX);
                cellY = floorf(cellY);
                cellZ = floorf(cellZ);        
                printf("delete child at %f %f %f\n", cellX, cellY, cellZ);

                voxel_model_delete_voxel(&app->voxelModel, (unsigned int)cellX, (unsigned int)cellY, (unsigned int)cellZ);
                return 1;
            }
        }
        break;
    case VOXEL_EDITOR_SHAPE_ID_CUBE:
        {
        Aabb aabb;
        aabb.x = cellX;
        aabb.y = cellY;
        aabb.z = cellZ;
        aabb.w = app->voxelEditAabbWidth;
        aabb.h = app->voxelEditAabbHeight;
        aabb.d = app->voxelEditAabbDepth;
        float maxX = aabb.x + aabb.w;
        float maxY = aabb.y + aabb.h;
        float maxZ = aabb.z + aabb.d;
        if(maxX >= 0.5 && maxY >= 0.5 && maxZ >= 0.5){
            voxel_model_delete_voxel_from_aabb(&app->voxelModel, aabb);
            return 1;
        }
        break;
        }
    case VOXEL_EDITOR_SHAPE_ID_SPHERE:
        {
            Sphere sphere;
            sphere.x = cellX + 0.5;
            sphere.y = cellY + 0.5;
            sphere.z = cellZ + 0.5;
            sphere.r = app->voxelEditSphereRadius;
            Aabb testBox = {0.5, 0.5, 0.5, 
                sphere.x + sphere.r + 0.5,
                sphere.y + sphere.r + 0.5, 
                sphere.z + sphere.r + 0.5
            };
            if(sphere_intersects_aabb(sphere, testBox)){
                voxel_model_delete_voxel_from_sphere(&app->voxelModel, sphere);
                return 1;
            }
        }
        break;
    }
    */

}

void app_update_ray_hit_result(App *app){
    float distance = 1000;
    int hitEntity = RAY_HIT_ENTITY_NONE;
    int hitSide = AABB_SIDE_LEFT;
    app->cursorRayDidHit = 0;

    if(!app->cursorRayDidHit){
        app_ray_vs_y0(&app, &app->cursorRayDidHit, app->cursorRay, &hitEntity, &distance);
        app_ray_vs_voxel_tree(&app, &app->cursorRayDidHit, app->mainVoxels, app->cursorRay, &hitEntity, &distance, &hitSide);
    } 
    app->cursorRayHitPoint[0] = app->cursorRay.x + app->cursorRay.dx * distance;
    app->cursorRayHitPoint[1] = app->cursorRay.y + app->cursorRay.dy * distance;
    app->cursorRayHitPoint[2] = app->cursorRay.z + app->cursorRay.dz * distance;   
    app->cursorHitEntity = hitEntity;
    app->cursorSideHit = hitSide;
}

void app_quit(App *app){
    app->shouldQuit = 1;
}

void app_set_action_bar_text(App *app, char *text){
    memset(app->actionBarText,0, sizeof(char) * 128);   
    int newLength = strcspn(text, "");
    memcpy(app->actionBarText, text, sizeof(char) * minf(newLength, 128));
}

void app_use_voxel_manipulator(App *app,float deltaTime){
    /*
    if(app->cursorRayDidHit){
        {
            char text[128];
            
            sprintf(text,"target: [x: %f, y: %f z: %f]", 
                app->cursorRayHitPoint[0],
                app->cursorRayHitPoint[1],
                app->cursorRayHitPoint[2]
            );
            app_set_action_bar_text(app,text);
        }

        int addSuccess = 0;
        int deleteSuccess = 0;
        
        
            
        switch (app->voxelEditorDynamicId)
        {
        case VOXEL_EDIT_DYNAMIC_PLOP:
            if(buttonState == BUTTON_STATE_PRESSED){
                switch (app->voxelEditModeId)
                {
                case VOXEL_EDIT_MODE_ID_ADD:
                    addSuccess = app_voxel_edit_mode_add(app);
                    break;
                case VOXEL_EDIT_MODE_ID_DELETE:
                    deleteSuccess = app_voxel_edit_mode_delete(app);
                    break;
                }
            }
            break;
        
        case VOXEL_EDIT_DYNAMIC_FLOW:   
            if(app->cursorButtonState[CURSOR_BUTTON_ID_PRIMARY] == CURSOR_BUTTON_STATE_PRESSED ||
                app->cursorButtonState[CURSOR_BUTTON_ID_PRIMARY] == CURSOR_BUTTON_STATE_DOWN){
                app->flowTick += deltaTime;
                float t = 1 / app->flowRate;
                if(app->flowTick >= t){
                    switch (app->voxelEditModeId)
                    {
                    case VOXEL_EDIT_MODE_ID_ADD:
                        addSuccess = app_voxel_edit_mode_add(app);
                        break;
                    case VOXEL_EDIT_MODE_ID_DELETE:
                        deleteSuccess = app_voxel_edit_mode_delete(app);
                        break;
                    }  
                    app->flowTick -= t;
                }                   
            }else{
                app->flowTick = 0;
            }
            break;
        }
        
        if(addSuccess || deleteSuccess){
            voxelTree->voxelCount = voxel_model_count_voxels(&voxelTree);
            if(voxelTree->voxelCount){
                voxel_model_update_from_tree(voxelModel, voxelTree);
            
                //app_update_voxel_model(app);
            
            }    
        }      
    }
    */
}




void app_reset_selection_tool(App *app){
    app->selectionTool.state = SELECTION_TOOL_STATE_PICK_ORIGIN;
}

void app_update_move_tool_pulley(App *app, vec3 origin){
    {
    vec3 offset = {6, 0, 0};
    vec3 halfExtents = {5, 0.2, 0.2};
    app->moveTool.xPulley = aabb_from_origin_halfExtents(origin, halfExtents);
    }
    {
        vec3 offset = {0, 6, 0};
        vec3 halfExtents = {0.2, 5, 0.2};
        app->moveTool.yPulley = aabb_from_origin_halfExtents(origin, halfExtents);
    }
    {
        vec3 offset = {0, 0, 6};
        vec3 halfExtents = {0.2, 0.2, 5};
        app->moveTool.zPulley = aabb_from_origin_halfExtents(origin, halfExtents);
    }

}

void app_update_box_scale_tool_pulley(App *app, Aabb box){
    vec3 origin;
    aabb_get_centre(box, origin);
    vec3 halfExtents = {0.5, 0.5, 0.5};

    {
        aabb_get_centre(box, origin);
        origin[0] += -box.w * 0.5 - 2;
        app->boxScaleTool.leftPulley= aabb_from_origin_halfExtents(origin, halfExtents);
    }
    //rightPulley
    {
        aabb_get_centre(box, origin);
        origin[0] += box.w * 0.5 + 2;
        app->boxScaleTool.rightPulley = aabb_from_origin_halfExtents(origin, halfExtents);
    }
    //bottomPulley
    {
        aabb_get_centre(box, origin);
        origin[1] += -box.h * 0.5 - 2;
        app->boxScaleTool.bottomPulley = aabb_from_origin_halfExtents(origin, halfExtents);
    }

    //topPulley
    {
        aabb_get_centre(box, origin);
        origin[1] += box.h * 0.5 + 2;
        app->boxScaleTool.topPulley = aabb_from_origin_halfExtents(origin, halfExtents);
    }
    //frontPulley
    {
        aabb_get_centre(box, origin);
        origin[2] += -box.d * 0.5 - 2;
        app->boxScaleTool.frontPulley = aabb_from_origin_halfExtents(origin, halfExtents);
    }

    //backPulley
    {
        aabb_get_centre(box, origin);
        origin[2] += box.d * 0.5 + 2;
        app->boxScaleTool.backPulley = aabb_from_origin_halfExtents(origin, halfExtents);
    }

}

void app_move_tool_update_model(App *app){
    {
        PosColor32Vertex verts[8];
        unsigned int elements[36];
        vec3 origin;
        vec3 halfExtents;
        aabb_get_centre(app->moveTool.xPulley, origin);
        vec3_scale(halfExtents, app->moveTool.xPulley.extents, 0.5);
        ColorRgbaF color = {1.0,0.0,0.0,1.0};
        box_generate_box_pos_color_32_vertex(origin,color,halfExtents,verts,elements);

        glBindBuffer(GL_ARRAY_BUFFER, app->vertexBufferId[VERTEX_BUFFER_X_MOVE_PULLEY]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PosColor32Vertex) * 8, verts, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->elementBufferId[ELEMENT_BUFFER_X_MOVE_PULLEY]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, elements, GL_DYNAMIC_DRAW);
    }

    
    {
        PosColor32Vertex verts[8];
        unsigned int elements[36];
        vec3 origin;
        vec3 halfExtents;
        aabb_get_centre(app->moveTool.yPulley, origin);
        vec3_scale(halfExtents, app->moveTool.yPulley.extents, 0.5);

        ColorRgbaF color = {0.0,1.0,0.0,1.0};
        box_generate_box_pos_color_32_vertex(origin,color,halfExtents,verts,elements);

        glBindBuffer(GL_ARRAY_BUFFER, app->vertexBufferId[VERTEX_BUFFER_Y_MOVE_PULLEY]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PosColor32Vertex) * 8, verts, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->elementBufferId[ELEMENT_BUFFER_Y_MOVE_PULLEY]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, elements, GL_DYNAMIC_DRAW);
    }
    
    {
        PosColor32Vertex verts[8];
        unsigned int elements[36];
        vec3 origin;
        vec3 halfExtents;
        aabb_get_centre(app->moveTool.zPulley, origin);
        vec3_scale(halfExtents, app->moveTool.zPulley.extents, 0.5);

        ColorRgbaF color = {0.0,0.0,1.0,1.0};
        box_generate_box_pos_color_32_vertex(origin,color,halfExtents,verts,elements);

        glBindBuffer(GL_ARRAY_BUFFER, app->vertexBufferId[VERTEX_BUFFER_Z_MOVE_PULLEY]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(PosColor32Vertex) * 8, verts, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, app->elementBufferId[ELEMENT_BUFFER_Z_MOVE_PULLEY]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, elements, GL_DYNAMIC_DRAW);
    }
}

void app_update_box_buffer(Aabb aabb, int* vertexBuffer, int* elementBuffer, ColorRgbaF color){
    PosColor32Vertex verts[8];
    unsigned int elements[36];
    vec3 origin;
    vec3 halfExtents;
    aabb_get_centre(aabb, origin);
    vec3_scale(halfExtents, aabb.extents, 0.5);
    box_generate_box_pos_color_32_vertex(origin,color,halfExtents,verts,elements);

    glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PosColor32Vertex) * 8, verts, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 36, elements, GL_DYNAMIC_DRAW);

}

void app_box_scale_tool_update_model(App *app){
            ColorRgbaF color = {1.0,0.5,1.0,1.0};

            app_update_box_buffer(app->boxScaleTool.leftPulley,&app->vertexBufferId[VERTEX_BUFFER_NX_BOX_SCALE_PULLEY],
                &app->elementBufferId[ELEMENT_BUFFER_NX_BOX_SCALE_PULLEY],color);

            app_update_box_buffer(app->boxScaleTool.rightPulley,&app->vertexBufferId[VERTEX_BUFFER_PX_BOX_SCALE_PULLEY],
                &app->elementBufferId[ELEMENT_BUFFER_PX_BOX_SCALE_PULLEY],color);

            app_update_box_buffer(app->boxScaleTool.bottomPulley,&app->vertexBufferId[VERTEX_BUFFER_NY_BOX_SCALE_PULLEY],
                &app->elementBufferId[ELEMENT_BUFFER_NY_BOX_SCALE_PULLEY],color);

            app_update_box_buffer(app->boxScaleTool.topPulley,&app->vertexBufferId[VERTEX_BUFFER_PY_BOX_SCALE_PULLEY],
                &app->elementBufferId[ELEMENT_BUFFER_PY_BOX_SCALE_PULLEY],color);
            
            app_update_box_buffer(app->boxScaleTool.frontPulley,&app->vertexBufferId[VERTEX_BUFFER_NZ_BOX_SCALE_PULLEY],
                &app->elementBufferId[ELEMENT_BUFFER_NZ_BOX_SCALE_PULLEY],color);

            app_update_box_buffer(app->boxScaleTool.backPulley,&app->vertexBufferId[VERTEX_BUFFER_PZ_BOX_SCALE_PULLEY],
                &app->elementBufferId[ELEMENT_BUFFER_PZ_BOX_SCALE_PULLEY],color);
        }