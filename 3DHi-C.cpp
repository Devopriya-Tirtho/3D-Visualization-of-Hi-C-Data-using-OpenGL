// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;
#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>

// Camera position and orientation
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Global scope or inside main function
int windowWidth = 800; // Example width
int windowHeight = 600; // Example height

float fov = 45.0f; // Default field of view
float aspectRatio = static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
float nearPlane = 0.1f; // Example value, can be adjusted
float farPlane = 100.0f; // Example value, can be adjusted

float pitch = 0.0f, yaw = -90.0f; // Initial pitch and yaw
float sensitivity = 0.1f; // Mouse sensitivity
//glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Initial camera direction



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) {
        fov = 1.0f;
    }
    else if (fov > 45.0f) {
        fov = 45.0f;
    }
}


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    float cameraSpeed = 0.05f; // Adjust as needed

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        cameraPos += cameraSpeed * cameraFront;
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        cameraPos -= cameraSpeed * cameraFront;
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}


int main( void )
{
    
    /* APPLICATION INITIALIZATION */

    
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 1024, 768, "CMPT 485", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    
    
    /* OPENGL INITIALIZATION */


	// Black background
	glClearColor(0.0f, 0.4f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

    // Set up callback functions
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);


	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

    GLuint lightPosID = glGetUniformLocation(programID, "lightPos");
    GLuint viewPosID = glGetUniformLocation(programID, "viewPos");

    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    
    /* LOAD MODEL INFORMATION */
    
    //glm::mat4 Model = glm::mat4(1.0f);  // Model matrix: an identity matrix (model will be at the origin)
    // = glm::translate(Model, glm::vec3(1.0f, 1.0f, 1.0f)); // Translate the model to the position (x, y, z)

    
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
    glm::mat4 View = glm::lookAt(
        glm::vec3(0, 50, -50), // Move the camera further away
        glm::vec3(0, 0, 0),
        glm::vec3(0, 1, 0)
    );
        
	// Model matrix : an identity matrix (model will be at the origin)
    
    /* TODO: SET MODEL MATRIX FROM INFO */
    
	//glm::mat4 Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	
    glm::mat4 MVP        = Projection * View; // Remember, matrix multiplication is the other way around

    
    /* CREATE VERTEX ATTRIBUTE ARRAYS */
    
    // Read our .obj file
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    std::vector<glm::ivec3> vertex_indices;
    std::vector<glm::ivec3> uv_indices;
    std::vector<glm::ivec3> normal_indices;
    
    // Load the OBJ file
    //std::vector<glm::vec3> vertices;
    //std::vector<glm::vec2> uvs;
    //std::vector<glm::vec3> normals;
    if (!loadOBJ("E:\\USask Graduate Study\\Computer Graphics\\NewProject\\cmpt485\\src\\meshes\\sphere.obj", vertices, uvs, normals)) {
        // Handle error in loading
        return -1;
    }
    
    // need to keep track of vbo sizes for drawing later
    GLsizei numVertices = (GLsizei)vertices.size(); // should be same as numNormals
    GLsizei numVertexIndices = (GLsizei)vertex_indices.size();

    /* CREATE VERTEX-BUFFER OBJECTS */

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
    
    GLuint normalsbuffer;
    glGenBuffers(1, &normalsbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer);
    glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    //instanced rendering
    std::vector<glm::vec3> instancePositions = {
       glm::vec3(0.501463f, 0.197946f, 0.006209f),
       glm::vec3(0.347137f, 0.969991f, 0.012417f),
       glm::vec3(0.346991f, 0.970171f, 0.018625f),
       glm::vec3(0.346691f, 0.970695f, 0.024832f),
       glm::vec3(0.346198f, 0.971424f, 0.031038f),
       glm::vec3(0.345561f, 0.972714f, 0.037243f),
       glm::vec3(0.344128f, 0.973846f, 0.043447f),
       glm::vec3(0.341900f, 0.975763f, 0.049649f),
       glm::vec3(0.339544f, 0.976998f, 0.055849f),
       glm::vec3(0.336263f, 0.978642f, 0.062047f),
       glm::vec3(0.332674f, 0.980112f, 0.068242f),
       glm::vec3(0.328244f, 0.981624f, 0.074435f),
       glm::vec3(0.323804f, 0.982905f, 0.080625f),
       glm::vec3(0.318745f, 0.984277f, 0.086812f),
       glm::vec3(0.313167f, 0.985717f, 0.092996f),
       glm::vec3(0.307348f, 0.987119f, 0.099176f),
       glm::vec3(0.301498f, 0.988432f, 0.105352f),
       glm::vec3(0.295045f, 0.989792f, 0.111524f),
       glm::vec3(0.288447f, 0.991072f, 0.117692f),
       glm::vec3(0.281437f, 0.992247f, 0.123855f),
       glm::vec3(0.274171f, 0.993415f, 0.130013f),
       glm::vec3(0.266603f, 0.994483f, 0.136167f),
       glm::vec3(0.258649f, 0.995471f, 0.142315f),
       glm::vec3(0.250566f, 0.996337f, 0.148458f),
       glm::vec3(0.241917f, 0.997430f, 0.154595f),
       glm::vec3(0.233651f, 0.997839f, 0.160726f),
       glm::vec3(0.224418f, 0.998605f, 0.166850f),
       glm::vec3(0.215441f, 0.998974f, 0.172969f),
       glm::vec3(0.206060f, 0.999441f, 0.179080f),
       glm::vec3(0.196500f, 0.999718f, 0.185185f),
       glm::vec3(0.186655f, 0.999895f, 0.191283f),
       glm::vec3(0.176580f, 1.000000f, 0.197373f),
       glm::vec3(0.166267f, 0.999993f, 0.203456f),
       glm::vec3(0.155733f, 0.999945f, 0.209531f),
       glm::vec3(0.145000f, 0.999745f, 0.215598f),
       glm::vec3(0.133996f, 0.999511f, 0.221656f),
       glm::vec3(0.122881f, 0.999142f, 0.227706f),
       glm::vec3(0.111478f, 0.998715f, 0.233747f),
       glm::vec3(0.099973f, 0.998161f, 0.239779f),
       glm::vec3(0.088205f, 0.997527f, 0.245802f),
       glm::vec3(0.076345f, 0.996774f, 0.251816f),
       glm::vec3(0.064231f, 0.995910f, 0.257819f),
       glm::vec3(0.052045f, 0.994932f, 0.263813f),
       glm::vec3(0.039605f, 0.993829f, 0.269797f),
       glm::vec3(0.027143f, 0.992636f, 0.275770f),
       glm::vec3(0.014437f, 0.991304f, 0.281733f),
       glm::vec3(0.001682f, 0.989909f, 0.287684f),
       glm::vec3(-0.011370f, 0.988354f, 0.293625f),
       glm::vec3(-0.024364f, 0.986655f, 0.299554f),
       glm::vec3(-0.037657f, 0.984883f, 0.305472f),
       glm::vec3(-0.050881f, 0.982989f, 0.311378f),
       glm::vec3(-0.064425f, 0.980843f, 0.317272f),
       glm::vec3(-0.077956f, 0.978687f, 0.323154f),
       glm::vec3(-0.091576f, 0.976322f, 0.329023f),
       glm::vec3(-0.105334f, 0.973839f, 0.334880f),
       glm::vec3(-0.119136f, 0.971224f, 0.340723f),
       glm::vec3(-0.133024f, 0.968469f, 0.346554f),
       glm::vec3(-0.146969f, 0.965582f, 0.352371f),
       glm::vec3(-0.160977f, 0.962543f, 0.358175f),
       glm::vec3(-0.175035f, 0.959374f, 0.363965f),
       glm::vec3(-0.189144f, 0.956040f, 0.369740f),
       glm::vec3(-0.203293f, 0.952579f, 0.375502f),
       glm::vec3(-0.217465f, 0.948943f, 0.381249f),
       glm::vec3(-0.231671f, 0.945175f, 0.386981f),
       glm::vec3(-0.245885f, 0.941228f, 0.392699f),
       glm::vec3(-0.260125f, 0.937148f, 0.398401f),
       glm::vec3(-0.274360f, 0.932893f, 0.404088f),
       glm::vec3(-0.288605f, 0.928501f, 0.409759f),
       glm::vec3(-0.302830f, 0.923940f, 0.415415f),
       glm::vec3(-0.317049f, 0.919231f, 0.421055f),
       glm::vec3(-0.331233f, 0.914365f, 0.426678f),
       glm::vec3(-0.345404f, 0.909340f, 0.432285f),
       glm::vec3(-0.359533f, 0.904157f, 0.437875f),
       glm::vec3(-0.373630f, 0.898814f, 0.443448f),
       glm::vec3(-0.387677f, 0.893312f, 0.449005f),
       glm::vec3(-0.401676f, 0.887638f, 0.454544f),
       glm::vec3(-0.415603f, 0.881821f, 0.460065f),
       glm::vec3(-0.429482f, 0.875820f, 0.465569f),
       glm::vec3(-0.443263f, 0.869696f, 0.471054f),
       glm::vec3(-0.456989f, 0.863378f, 0.476522f),
       glm::vec3(-0.470608f, 0.856935f, 0.481971f),
       glm::vec3(-0.484146f, 0.850310f, 0.487402f),
       glm::vec3(-0.497569f, 0.843553f, 0.492814f),
       glm::vec3(-0.510883f, 0.836628f, 0.498207f),
       glm::vec3(-0.524075f, 0.829559f, 0.503580f),
       glm::vec3(-0.537146f, 0.822335f, 0.508935f),
       glm::vec3(-0.550078f, 0.814966f, 0.514269f),
       glm::vec3(-0.562875f, 0.807446f, 0.519584f),
       glm::vec3(-0.575513f, 0.799780f, 0.524879f),
       glm::vec3(-0.588010f, 0.791970f, 0.530153f),
       glm::vec3(-0.600330f, 0.784009f, 0.535407f),
       glm::vec3(-0.612498f, 0.775920f, 0.540641f),
       glm::vec3(-0.624476f, 0.767670f, 0.545853f),
       glm::vec3(-0.636278f, 0.759313f, 0.551045f),
       glm::vec3(-0.647879f, 0.750787f, 0.556215f),
       glm::vec3(-0.659296f, 0.742148f, 0.561364f),
       glm::vec3(-0.670510f, 0.733342f, 0.566492f),
       glm::vec3(-0.681524f, 0.724426f, 0.571597f),
       glm::vec3(-0.692313f, 0.715358f, 0.576680f),
       glm::vec3(-0.702887f, 0.706178f, 0.581741f),
       glm::vec3(-0.713228f, 0.696858f, 0.586780f),
       glm::vec3(-0.723332f, 0.687420f, 0.591796f),
       glm::vec3(-0.733195f, 0.677853f, 0.596790f),
       glm::vec3(-0.742806f, 0.668174f, 0.601760f),
       glm::vec3(-0.752165f, 0.658370f, 0.606707f),
       glm::vec3(-0.761233f, 0.648458f, 0.611631f),
       glm::vec3(-0.769976f, 0.638437f, 0.616531f),
       glm::vec3(-0.778388f, 0.628264f, 0.621407f),
       glm::vec3(-0.786707f, 0.618210f, 0.626260f),
       glm::vec3(-0.794841f, 0.607720f, 0.631088f),
       glm::vec3(-0.802446f, 0.597227f, 0.635892f),
       glm::vec3(-0.809797f, 0.586881f, 0.640671f),
       glm::vec3(-0.816771f, 0.576291f, 0.645426f),
       glm::vec3(-0.823544f, 0.565434f, 0.650156f),
       glm::vec3(-0.829927f, 0.554829f, 0.654861f),
       glm::vec3(-0.835797f, 0.543912f, 0.659540f),
       glm::vec3(-0.841244f, 0.532982f, 0.664194f),
       glm::vec3(-0.846554f, 0.521910f, 0.668823f),
       glm::vec3(-0.851459f, 0.510876f, 0.673426f),
       glm::vec3(-0.856043f, 0.499684f, 0.678002f),
       glm::vec3(-0.860198f, 0.488498f, 0.682553f),
       glm::vec3(-0.864020f, 0.477214f, 0.687077f),
       glm::vec3(-0.867416f, 0.465895f, 0.691575f),
       glm::vec3(-0.870461f, 0.454516f, 0.696047f),
       glm::vec3(-0.873096f, 0.443085f, 0.700491f),
       glm::vec3(-0.875340f, 0.431622f, 0.704908f),
       glm::vec3(-0.877178f, 0.420111f, 0.709298f),
       glm::vec3(-0.878598f, 0.408575f, 0.713661f),
       glm::vec3(-0.879621f, 0.397002f, 0.717997f),
       glm::vec3(-0.880200f, 0.385388f, 0.722304f),
       glm::vec3(-0.880380f, 0.373758f, 0.726584f),
       glm::vec3(-0.880099f, 0.362089f, 0.730836f),
       glm::vec3(-0.879401f, 0.350401f, 0.735060f),
       glm::vec3(-0.878216f, 0.338681f, 0.739255f),
       glm::vec3(-0.876587f, 0.326933f, 0.743422f),
       glm::vec3(-0.874470f, 0.315177f, 0.747560f),
       glm::vec3(-0.871950f, 0.303449f, 0.751669f),
       glm::vec3(-0.868989f, 0.291689f, 0.755750f),
       glm::vec3(-0.865371f, 0.279853f, 0.759801f),
       glm::vec3(-0.861617f, 0.268407f, 0.763823f),
       glm::vec3(-0.857505f, 0.256936f, 0.767815f),
       glm::vec3(-0.853099f, 0.245852f, 0.771778f),
       glm::vec3(-0.848435f, 0.234958f, 0.775711f),
       glm::vec3(-0.843482f, 0.224280f, 0.779615f),
       glm::vec3(-0.838278f, 0.213846f, 0.783488f),
       glm::vec3(-0.832845f, 0.203648f, 0.787331f),
       glm::vec3(-0.827133f, 0.193695f, 0.791143f),
       glm::vec3(-0.821196f, 0.183974f, 0.794926f),
       glm::vec3(-0.814973f, 0.174499f, 0.798677f),
       glm::vec3(-0.808537f, 0.165257f, 0.802398f),
       glm::vec3(-0.801818f, 0.156267f, 0.806088f),
       glm::vec3(-0.794891f, 0.147512f, 0.809746f),
       glm::vec3(-0.787684f, 0.139016f, 0.813374f),
       glm::vec3(-0.780269f, 0.130751f, 0.816970f),
       glm::vec3(-0.772592f, 0.122768f, 0.820535f),
       glm::vec3(-0.764697f, 0.115014f, 0.824068f),
       glm::vec3(-0.756548f, 0.107540f, 0.827569f),
       glm::vec3(-0.748149f, 0.100280f, 0.831038f),
       glm::vec3(-0.739514f, 0.093293f, 0.834476f),
       glm::vec3(-0.730688f, 0.086617f, 0.837881f),
       glm::vec3(-0.721610f, 0.080266f, 0.841254f),
       glm::vec3(-0.712050f, 0.074063f, 0.844594f),
       glm::vec3(-0.702911f, 0.068375f, 0.847902f),
       glm::vec3(-0.693568f, 0.063095f, 0.851177f),
       glm::vec3(-0.684087f, 0.058254f, 0.854419f),
       glm::vec3(-0.674502f, 0.053840f, 0.857629f),
       glm::vec3(-0.664931f, 0.049865f, 0.860805f),
       glm::vec3(-0.655232f, 0.046296f, 0.863948f),
       glm::vec3(-0.645534f, 0.043206f, 0.867058f),
       glm::vec3(-0.635716f, 0.040515f, 0.870135f),
       glm::vec3(-0.625914f, 0.038299f, 0.873178f),
       glm::vec3(-0.615992f, 0.036478f, 0.876187f),
       glm::vec3(-0.606084f, 0.035121f, 0.879163f),
       glm::vec3(-0.596083f, 0.034152f, 0.882104f),
       glm::vec3(-0.586069f, 0.033622f, 0.885012f),
       glm::vec3(-0.576009f, 0.033477f, 0.887885f),
       glm::vec3(-0.565907f, 0.033734f, 0.890724f),
       glm::vec3(-0.555779f, 0.034389f, 0.893529f),
       glm::vec3(-0.545608f, 0.035412f, 0.896300f),
       glm::vec3(-0.535406f, 0.036832f, 0.899036f),
       glm::vec3(-0.525152f, 0.038606f, 0.901737f),
       glm::vec3(-0.514881f, 0.040761f, 0.904404f),
       glm::vec3(-0.504551f, 0.043249f, 0.907035f),
       glm::vec3(-0.494202f, 0.046105f, 0.909632f),
       glm::vec3(-0.483833f, 0.049295f, 0.912194f),
       glm::vec3(-0.473450f, 0.052809f, 0.914720f),
       glm::vec3(-0.462976f, 0.056698f, 0.917211f),
       glm::vec3(-0.452572f, 0.060860f, 0.919667f),
       glm::vec3(-0.442083f, 0.065359f, 0.922088f),
       glm::vec3(-0.431594f, 0.070173f, 0.924472f),
       glm::vec3(-0.421070f, 0.075295f, 0.926822f),
       glm::vec3(-0.410538f, 0.080682f, 0.929135f),
       glm::vec3(-0.399962f, 0.086387f, 0.931413f),
       glm::vec3(-0.389356f, 0.092337f, 0.933655f),
       glm::vec3(-0.378718f, 0.098501f, 0.935860f),
       glm::vec3(-0.368051f, 0.104992f, 0.938030f),
       glm::vec3(-0.357374f, 0.111791f, 0.940164f),
       glm::vec3(-0.346733f, 0.118531f, 0.942261f),
       glm::vec3(-0.336260f, 0.125448f, 0.944322f),
       glm::vec3(-0.325920f, 0.132548f, 0.946346f),
       glm::vec3(-0.315664f, 0.139671f, 0.948335f),
       glm::vec3(-0.305552f, 0.146890f, 0.950286f),
       glm::vec3(-0.295561f, 0.154194f, 0.952201f),
       glm::vec3(-0.285730f, 0.161601f, 0.954079f),
       glm::vec3(-0.276026f, 0.169033f, 0.955921f),
       glm::vec3(-0.266428f, 0.176594f, 0.957725f),
       glm::vec3(-0.256997f, 0.184129f, 0.959493f),
       glm::vec3(-0.247625f, 0.191778f, 0.961224f),
       glm::vec3(-0.238446f, 0.199397f, 0.962917f),
       glm::vec3(-0.229297f, 0.207118f, 0.964574f),
       glm::vec3(-0.220346f, 0.214827f, 0.966193f),
       glm::vec3(-0.211441f, 0.222621f, 0.967775f),
       glm::vec3(-0.202731f, 0.230409f, 0.969320f),
       glm::vec3(-0.194056f, 0.238241f, 0.970827f),
       glm::vec3(-0.185554f, 0.246087f, 0.972297f),
       glm::vec3(-0.177103f, 0.253927f, 0.973730f),
       glm::vec3(-0.168785f, 0.261795f, 0.975125f),
       glm::vec3(-0.160554f, 0.269620f, 0.976482f),
       glm::vec3(-0.152413f, 0.277473f, 0.977802f),
       glm::vec3(-0.144396f, 0.285286f, 0.979084f),
       glm::vec3(-0.136447f, 0.293086f, 0.980328f),
       glm::vec3(-0.128616f, 0.300842f, 0.981535f),
       glm::vec3(-0.120841f, 0.308570f, 0.982704f),
       glm::vec3(-0.113179f, 0.316235f, 0.983834f),
       glm::vec3(-0.105546f, 0.323866f, 0.984927f),
       glm::vec3(-0.098031f, 0.331434f, 0.985982f),
       glm::vec3(-0.090580f, 0.338928f, 0.986999f),
       glm::vec3(-0.083231f, 0.346344f, 0.987978f),
       glm::vec3(-0.075893f, 0.353720f, 0.988919f),
       glm::vec3(-0.068676f, 0.360984f, 0.989821f),
       glm::vec3(-0.061539f, 0.368166f, 0.990686f),
       glm::vec3(-0.054431f, 0.375245f, 0.991512f),
       glm::vec3(-0.047386f, 0.382237f, 0.992300f),
       glm::vec3(-0.040403f, 0.389091f, 0.993050f),
       glm::vec3(-0.033469f, 0.395811f, 0.993762f),
       glm::vec3(-0.026565f, 0.402406f, 0.994435f),
       glm::vec3(-0.019740f, 0.408844f, 0.995070f),
       glm::vec3(-0.012935f, 0.415173f, 0.995667f),
       glm::vec3(-0.006189f, 0.421324f, 0.996225f),
       glm::vec3(0.000535f, 0.427327f, 0.996744f),
       glm::vec3(0.007222f, 0.433136f, 0.997226f),
       glm::vec3(0.013865f, 0.438813f, 0.997669f),
       glm::vec3(0.020515f, 0.444248f, 0.998073f),
       glm::vec3(0.027159f, 0.449523f, 0.998439f),
       glm::vec3(0.033767f, 0.454544f, 0.998767f),
       glm::vec3(0.040367f, 0.459352f, 0.999056f),
       glm::vec3(0.046982f, 0.463919f, 0.999306f),
       glm::vec3(0.053567f, 0.468231f, 0.999518f),
       glm::vec3(0.060185f, 0.472291f, 0.999692f),
       glm::vec3(0.066772f, 0.476074f, 0.999827f),
       glm::vec3(0.073397f, 0.479604f, 0.999923f),
       glm::vec3(0.079985f, 0.482793f, 0.999981f),
       glm::vec3(0.086618f, 0.485748f, 1.000000f),
       glm::vec3(0.093198f, 0.488336f, 0.999981f),
       glm::vec3(0.099826f, 0.490671f, 0.999923f),
       glm::vec3(0.106397f, 0.492640f, 0.999827f),
       glm::vec3(0.112995f, 0.494314f, 0.999692f),
       glm::vec3(0.119537f, 0.495649f, 0.999518f),
       glm::vec3(0.126080f, 0.496628f, 0.999306f),
       glm::vec3(0.132558f, 0.497316f, 0.999056f),
       glm::vec3(0.139019f, 0.497631f, 0.998767f),
       glm::vec3(0.145428f, 0.497646f, 0.998439f),
       glm::vec3(0.151757f, 0.497294f, 0.998073f),
       glm::vec3(0.158090f, 0.496640f, 0.997669f),
       glm::vec3(0.164258f, 0.495617f, 0.997226f),
       glm::vec3(0.170441f, 0.494294f, 0.996744f),
       glm::vec3(0.176493f, 0.492595f, 0.996225f),
       glm::vec3(0.182518f, 0.490569f, 0.995667f),
       glm::vec3(0.188370f, 0.488218f, 0.995070f),
       glm::vec3(0.194165f, 0.485501f, 0.994435f),
       glm::vec3(0.199761f, 0.482539f, 0.993762f),
       glm::vec3(0.205285f, 0.479195f, 0.993050f),
       glm::vec3(0.210619f, 0.475654f, 0.992300f),
       glm::vec3(0.215870f, 0.471774f, 0.991512f),
       glm::vec3(0.220968f, 0.467709f, 0.990686f),
       glm::vec3(0.225932f, 0.463372f, 0.989821f),
       glm::vec3(0.230830f, 0.458793f, 0.988919f),
       glm::vec3(0.235517f, 0.454025f, 0.987978f),
       glm::vec3(0.240183f, 0.449017f, 0.986999f),
       glm::vec3(0.244613f, 0.443819f, 0.985982f),
       glm::vec3(0.248989f, 0.438397f, 0.984927f),
       glm::vec3(0.253203f, 0.432832f, 0.983834f),
       glm::vec3(0.257301f, 0.427045f, 0.982704f),
       glm::vec3(0.261302f, 0.421164f, 0.981535f),
       glm::vec3(0.265174f, 0.415068f, 0.980328f),
       glm::vec3(0.268945f, 0.408917f, 0.979084f),
       glm::vec3(0.272679f, 0.402574f, 0.977802f),
       glm::vec3(0.276309f, 0.396138f, 0.976482f),
       glm::vec3(0.279890f, 0.389588f, 0.975125f),
       glm::vec3(0.283395f, 0.382943f, 0.973730f),
       glm::vec3(0.286874f, 0.376188f, 0.972297f),
       glm::vec3(0.290265f, 0.369386f, 0.970827f),
       glm::vec3(0.293666f, 0.362444f, 0.969320f),
       glm::vec3(0.296998f, 0.355492f, 0.967775f),
       glm::vec3(0.300330f, 0.348418f, 0.966193f),
       glm::vec3(0.303610f, 0.341283f, 0.964574f),
       glm::vec3(0.306924f, 0.334107f, 0.962917f),
       glm::vec3(0.310126f, 0.326898f, 0.961224f),
       glm::vec3(0.313377f, 0.319577f, 0.959493f),
       glm::vec3(0.316575f, 0.312318f, 0.957725f),
       glm::vec3(0.319811f, 0.304969f, 0.955921f),
       glm::vec3(0.323007f, 0.297650f, 0.954079f),
       glm::vec3(0.326291f, 0.290299f, 0.952201f),
       glm::vec3(0.329519f, 0.282960f, 0.950286f),
       glm::vec3(0.332867f, 0.275576f, 0.948335f),
       glm::vec3(0.336159f, 0.268263f, 0.946346f),
       glm::vec3(0.339528f, 0.260921f, 0.944322f),
       glm::vec3(0.342900f, 0.253625f, 0.942261f),
       glm::vec3(0.346341f, 0.246319f, 0.940164f),
       glm::vec3(0.349816f, 0.239060f, 0.938030f),
       glm::vec3(0.353360f, 0.231851f, 0.935860f),
       glm::vec3(0.356941f, 0.224652f, 0.933655f),
       glm::vec3(0.360629f, 0.217550f, 0.931413f),
       glm::vec3(0.364348f, 0.210488f, 0.929135f),
       glm::vec3(0.368147f, 0.203503f, 0.926822f),
       glm::vec3(0.372068f, 0.196592f, 0.924472f),
       glm::vec3(0.375999f, 0.189774f, 0.922088f),
       glm::vec3(0.380082f, 0.182968f, 0.919667f),
       glm::vec3(0.384239f, 0.176367f, 0.917211f),
       glm::vec3(0.388544f, 0.169800f, 0.914720f),
       glm::vec3(0.392905f, 0.163390f, 0.912194f),
       glm::vec3(0.397434f, 0.157089f, 0.909632f),
       glm::vec3(0.402010f, 0.150906f, 0.907035f),
       glm::vec3(0.406809f, 0.144843f, 0.904404f),
       glm::vec3(0.411621f, 0.138914f, 0.901737f),
       glm::vec3(0.416608f, 0.133090f, 0.899036f),
       glm::vec3(0.421659f, 0.127395f, 0.896300f),
       glm::vec3(0.426869f, 0.121837f, 0.893529f),
       glm::vec3(0.432159f, 0.116415f, 0.890724f),
       glm::vec3(0.437594f, 0.111162f, 0.887885f),
       glm::vec3(0.443165f, 0.106033f, 0.885012f),
       glm::vec3(0.448847f, 0.101107f, 0.882104f),
       glm::vec3(0.454710f, 0.096337f, 0.879163f),
       glm::vec3(0.460644f, 0.091784f, 0.876187f),
       glm::vec3(0.466802f, 0.087396f, 0.873178f),
       glm::vec3(0.473014f, 0.083232f, 0.870135f),
       glm::vec3(0.479434f, 0.079259f, 0.867058f),
       glm::vec3(0.485920f, 0.075509f, 0.863948f),
       glm::vec3(0.492597f, 0.071961f, 0.860805f),
       glm::vec3(0.499317f, 0.068646f, 0.857629f),
       glm::vec3(0.506214f, 0.065535f, 0.854419f),
       glm::vec3(0.513164f, 0.062664f, 0.851177f),
       glm::vec3(0.520271f, 0.059976f, 0.847902f),
       glm::vec3(0.527431f, 0.057551f, 0.844594f),
       glm::vec3(0.534724f, 0.055294f, 0.841254f),
       glm::vec3(0.542035f, 0.053313f, 0.837881f),
       glm::vec3(0.549481f, 0.051485f, 0.834476f),
       glm::vec3(0.556912f, 0.049946f, 0.831038f),
       glm::vec3(0.564456f, 0.048547f, 0.827569f),
       glm::vec3(0.571982f, 0.047443f, 0.824068f),
       glm::vec3(0.579620f, 0.046472f, 0.820535f),
       glm::vec3(0.587225f, 0.045799f, 0.816970f),
       glm::vec3(0.594908f, 0.045260f, 0.813374f),
       glm::vec3(0.602540f, 0.045006f, 0.809746f),
       glm::vec3(0.610236f, 0.044886f, 0.806088f),
       glm::vec3(0.617878f, 0.045049f, 0.802398f),
       glm::vec3(0.625582f, 0.045335f, 0.798677f),
       glm::vec3(0.633225f, 0.045884f, 0.794926f),
       glm::vec3(0.640904f, 0.046564f, 0.791143f),
       glm::vec3(0.648526f, 0.047491f, 0.787331f),
       glm::vec3(0.656166f, 0.048564f, 0.783488f),
       glm::vec3(0.663721f, 0.049850f, 0.779615f),
       glm::vec3(0.671295f, 0.051305f, 0.775711f),
       glm::vec3(0.678786f, 0.052948f, 0.771778f),
       glm::vec3(0.686299f, 0.054790f, 0.767815f),
       glm::vec3(0.693706f, 0.056796f, 0.763823f),
       glm::vec3(0.701136f, 0.059026f, 0.759801f),
       glm::vec3(0.708402f, 0.061384f, 0.755750f),
       glm::vec3(0.715740f, 0.063953f, 0.751669f),
       glm::vec3(0.722953f, 0.066647f, 0.747560f),
       glm::vec3(0.730121f, 0.069551f, 0.743422f),
       glm::vec3(0.737190f, 0.072625f, 0.739255f),
       glm::vec3(0.744188f, 0.075851f, 0.735060f),
       glm::vec3(0.751181f, 0.079245f, 0.730836f),
       glm::vec3(0.758078f, 0.082798f, 0.726584f),
       glm::vec3(0.764899f, 0.086500f, 0.722304f),
       glm::vec3(0.771815f, 0.090359f, 0.717997f),
       glm::vec3(0.778943f, 0.094390f, 0.713661f),
       glm::vec3(0.786267f, 0.098572f, 0.709298f),
       glm::vec3(0.793769f, 0.102959f, 0.704908f),
       glm::vec3(0.801332f, 0.107511f, 0.700491f),
       glm::vec3(0.808861f, 0.112241f, 0.696047f),
       glm::vec3(0.816437f, 0.117125f, 0.691575f),
       glm::vec3(0.823945f, 0.122197f, 0.687077f),
       glm::vec3(0.831505f, 0.127377f, 0.682553f),
       glm::vec3(0.838920f, 0.132733f, 0.678002f),
       glm::vec3(0.846077f, 0.138182f, 0.673426f),
       glm::vec3(0.853030f, 0.143754f, 0.668823f),
       glm::vec3(0.859773f, 0.149425f, 0.664194f),
       glm::vec3(0.866299f, 0.155245f, 0.659540f),
       glm::vec3(0.872560f, 0.161113f, 0.654861f),
       glm::vec3(0.878621f, 0.167142f, 0.650156f),
       glm::vec3(0.884412f, 0.173223f, 0.645426f),
       glm::vec3(0.890010f, 0.179444f, 0.640671f),
       glm::vec3(0.895345f, 0.185673f, 0.635892f),
       glm::vec3(0.900430f, 0.192073f, 0.631088f),
       glm::vec3(0.905316f, 0.198485f, 0.626260f),
       glm::vec3(0.909977f, 0.205043f, 0.621407f),
       glm::vec3(0.914442f, 0.211596f, 0.616531f),
       glm::vec3(0.918633f, 0.218252f, 0.611631f),
       glm::vec3(0.922674f, 0.224929f, 0.606707f),
       glm::vec3(0.926470f, 0.231719f, 0.601760f),
       glm::vec3(0.930062f, 0.238536f, 0.596790f),
       glm::vec3(0.933372f, 0.245343f, 0.591796f),
       glm::vec3(0.936489f, 0.252213f, 0.586780f),
       glm::vec3(0.939272f, 0.259050f, 0.581741f),
       glm::vec3(0.941835f, 0.265950f, 0.576680f),
       glm::vec3(0.944092f, 0.272767f, 0.571597f),
       glm::vec3(0.946106f, 0.279614f, 0.566492f),
       glm::vec3(0.947851f, 0.286404f, 0.561364f),
       glm::vec3(0.949297f, 0.293231f, 0.556215f),
       glm::vec3(0.950499f, 0.299946f, 0.551045f),
       glm::vec3(0.951430f, 0.306646f, 0.545853f),
       glm::vec3(0.952114f, 0.313251f, 0.540641f),
       glm::vec3(0.952488f, 0.319776f, 0.535407f),
       glm::vec3(0.952645f, 0.326232f, 0.530153f),
       glm::vec3(0.952507f, 0.332565f, 0.524879f),
       glm::vec3(0.952122f, 0.338833f, 0.519584f),
       glm::vec3(0.951475f, 0.344902f, 0.514269f),
       glm::vec3(0.950541f, 0.350923f, 0.508935f),
       glm::vec3(0.949390f, 0.356712f, 0.503580f),
       glm::vec3(0.947910f, 0.362439f, 0.498207f),
       glm::vec3(0.946223f, 0.367851f, 0.492814f),
       glm::vec3(0.944180f, 0.373189f, 0.487402f),
       glm::vec3(0.941906f, 0.378134f, 0.481971f),
       glm::vec3(0.939291f, 0.382968f, 0.476522f),
       glm::vec3(0.936505f, 0.387397f, 0.471054f),
       glm::vec3(0.933192f, 0.391866f, 0.465569f),
       glm::vec3(0.929948f, 0.395839f, 0.460065f),
       glm::vec3(0.926239f, 0.399736f, 0.454544f),
       glm::vec3(0.922530f, 0.403170f, 0.449005f),
       glm::vec3(0.918580f, 0.406359f, 0.443448f),
       glm::vec3(0.914715f, 0.408567f, 0.437875f),
       glm::vec3(0.910078f, 0.410883f, 0.432285f),
       glm::vec3(0.905722f, 0.412602f, 0.426678f),
       glm::vec3(0.901064f, 0.413635f, 0.421055f),
       glm::vec3(0.896398f, 0.414451f, 0.415415f),
       glm::vec3(0.891474f, 0.414736f, 0.409759f),
       glm::vec3(0.886697f, 0.414604f, 0.404088f),
       glm::vec3(0.881725f, 0.413835f, 0.398401f),
       glm::vec3(0.877080f, 0.412620f, 0.392699f),
       glm::vec3(0.872337f, 0.410871f, 0.386981f),
       glm::vec3(0.867814f, 0.408528f, 0.381249f),
       glm::vec3(0.863392f, 0.405850f, 0.375502f),
       glm::vec3(0.859198f, 0.402866f, 0.369740f),
       glm::vec3(0.855115f, 0.399340f, 0.363965f),
       glm::vec3(0.851258f, 0.395544f, 0.358175f),
       glm::vec3(0.847581f, 0.391247f, 0.352371f),
       glm::vec3(0.844079f, 0.386668f, 0.346554f),
       glm::vec3(0.840790f, 0.381613f, 0.340723f),
       glm::vec3(0.837630f, 0.376228f, 0.334880f),
       glm::vec3(0.834765f, 0.370509f, 0.329023f),
       glm::vec3(0.832062f, 0.364448f, 0.323154f),
       glm::vec3(0.829603f, 0.358060f, 0.317272f),
       glm::vec3(0.827361f, 0.351335f, 0.311378f),
       glm::vec3(0.825324f, 0.344345f, 0.305472f),
       glm::vec3(0.823507f, 0.336991f, 0.299554f),
       glm::vec3(0.821902f, 0.329406f, 0.293625f),
       glm::vec3(0.820507f, 0.321458f, 0.287684f),
       glm::vec3(0.819316f, 0.313321f, 0.281733f),
       glm::vec3(0.818329f, 0.304799f, 0.275770f),
       glm::vec3(0.817549f, 0.296083f, 0.269797f),
       glm::vec3(0.816984f, 0.287028f, 0.263813f),
       glm::vec3(0.816642f, 0.277772f, 0.257819f),
       glm::vec3(0.816480f, 0.268158f, 0.251816f),
       glm::vec3(0.816546f, 0.258346f, 0.245802f),
       glm::vec3(0.816819f, 0.248183f, 0.239779f),
       glm::vec3(0.817281f, 0.237847f, 0.233747f),
       glm::vec3(0.817955f, 0.227180f, 0.227706f),
       glm::vec3(0.818796f, 0.216333f, 0.221656f),
       glm::vec3(0.819820f, 0.205164f, 0.215598f),
       glm::vec3(0.821021f, 0.193879f, 0.209531f),
       glm::vec3(0.822377f, 0.182273f, 0.203456f),
       glm::vec3(0.823953f, 0.170486f, 0.197373f),
       glm::vec3(0.825738f, 0.158331f, 0.191283f),
       glm::vec3(0.827584f, 0.146112f, 0.185185f),
       glm::vec3(0.829696f, 0.133494f, 0.179080f),
       glm::vec3(0.832009f, 0.120730f, 0.172969f),
       glm::vec3(0.834261f, 0.107820f, 0.166850f),
       glm::vec3(0.836447f, 0.095144f, 0.160726f),
       glm::vec3(0.838678f, 0.082105f, 0.154595f),
       glm::vec3(0.840716f, 0.069283f, 0.148458f),
       glm::vec3(0.842883f, 0.056440f, 0.142315f),
       glm::vec3(0.844973f, 0.043434f, 0.136167f),
       glm::vec3(0.847048f, 0.030479f, 0.130013f),
       glm::vec3(0.849088f, 0.017493f, 0.123855f),
       glm::vec3(0.851123f, 0.004453f, 0.117692f),
       glm::vec3(0.853043f, -0.008547f, 0.111524f),
       glm::vec3(0.855004f, -0.021618f, 0.105352f),
       glm::vec3(0.856845f, -0.034649f, 0.099176f),
       glm::vec3(0.858688f, -0.047756f, 0.092996f),
       glm::vec3(0.860474f, -0.060769f, 0.086812f),
       glm::vec3(0.862235f, -0.073910f, 0.080625f),
       glm::vec3(0.863938f, -0.086942f, 0.074435f),
       glm::vec3(0.865631f, -0.100049f, 0.068242f),
       glm::vec3(0.867196f, -0.113083f, 0.062047f),
       glm::vec3(0.868774f, -0.126155f, 0.055849f),
       glm::vec3(0.870201f, -0.139134f, 0.049649f),
       glm::vec3(0.871578f, -0.152133f, 0.043447f),
       glm::vec3(0.872846f, -0.165079f, 0.037243f),
       glm::vec3(0.874136f, -0.177953f, 0.031038f),
       glm::vec3(0.875292f, -0.190773f, 0.024832f),
       glm::vec3(0.876385f, -0.203576f, 0.018625f),
       glm::vec3(0.877454f, -0.216245f, 0.012417f),
       glm::vec3(0.878486f, -0.228850f, 0.006209f),
       glm::vec3(0.879430f, -0.241403f, 0.000000f),
       glm::vec3(0.880356f, -0.253847f, -0.006209f),
       glm::vec3(0.881218f, -0.266199f, -0.012417f),
       glm::vec3(0.882070f, -0.278439f, -0.018625f),
       glm::vec3(0.882863f, -0.290581f, -0.024832f),
       glm::vec3(0.883646f, -0.302606f, -0.031038f),
       glm::vec3(0.884386f, -0.314521f, -0.037243f),
       glm::vec3(0.885121f, -0.326304f, -0.043447f),
       glm::vec3(0.885813f, -0.337975f, -0.049649f),
       glm::vec3(0.886511f, -0.349528f, -0.055849f),
       glm::vec3(0.887177f, -0.360962f, -0.062047f),
       glm::vec3(0.887848f, -0.372273f, -0.068242f),
       glm::vec3(0.888524f, -0.383435f, -0.074435f),
       glm::vec3(0.889187f, -0.394451f, -0.080625f),
       glm::vec3(0.889888f, -0.405391f, -0.086812f),
       glm::vec3(0.890644f, -0.416123f, -0.092996f),
       glm::vec3(0.891395f, -0.426768f, -0.099176f),
       glm::vec3(0.892188f, -0.437266f, -0.105352f),
       glm::vec3(0.893005f, -0.447637f, -0.111524f),
       glm::vec3(0.893819f, -0.457873f, -0.117692f),
       glm::vec3(0.894673f, -0.467989f, -0.123855f),
       glm::vec3(0.895512f, -0.477953f, -0.130013f),
       glm::vec3(0.896406f, -0.487797f, -0.136167f),
       glm::vec3(0.897271f, -0.497503f, -0.142315f),
       glm::vec3(0.898199f, -0.507078f, -0.148458f),
       glm::vec3(0.899109f, -0.516525f, -0.154595f),
       glm::vec3(0.900088f, -0.525825f, -0.160726f),
       glm::vec3(0.901051f, -0.535011f, -0.166850f),
       glm::vec3(0.902093f, -0.544041f, -0.172969f),
       glm::vec3(0.903154f, -0.552957f, -0.179080f),
       glm::vec3(0.904269f, -0.561686f, -0.185185f),
       glm::vec3(0.905486f, -0.570332f, -0.191283f),
       glm::vec3(0.906685f, -0.578734f, -0.197373f),
       glm::vec3(0.908012f, -0.587076f, -0.203456f),
       glm::vec3(0.909343f, -0.595136f, -0.209531f),
       glm::vec3(0.910787f, -0.603098f, -0.215598f),
       glm::vec3(0.912240f, -0.610814f, -0.221656f),
       glm::vec3(0.913774f, -0.618437f, -0.227706f),
       glm::vec3(0.915275f, -0.625798f, -0.233747f),
       glm::vec3(0.916761f, -0.632775f, -0.239779f),
       glm::vec3(0.917936f, -0.639166f, -0.245802f),
       glm::vec3(0.919122f, -0.645282f, -0.251816f),
       glm::vec3(0.920188f, -0.651028f, -0.257819f),
       glm::vec3(0.921143f, -0.656249f, -0.263813f),
       glm::vec3(0.922069f, -0.660887f, -0.269797f),
       glm::vec3(0.922833f, -0.665302f, -0.275770f),
       glm::vec3(0.923552f, -0.669040f, -0.281733f),
       glm::vec3(0.924244f, -0.672511f, -0.287684f),
       glm::vec3(0.924472f, -0.674889f, -0.293625f),
       glm::vec3(0.925006f, -0.677649f, -0.299554f),
       glm::vec3(0.925008f, -0.678749f, -0.305472f),
       glm::vec3(0.924822f, -0.680521f, -0.311378f),
       glm::vec3(0.924764f, -0.680084f, -0.317272f),
       //Chromosome 2
       glm::vec3(0.890286f, 0.631685f, -0.323154f),
       glm::vec3(0.999390f, -0.568106f, -0.329023f),
       glm::vec3(0.999170f, -0.567794f, -0.334880f),
       glm::vec3(1.000000f, -0.569082f, -0.340723f),
       glm::vec3(0.998965f, -0.569289f, -0.346554f),
       glm::vec3(0.998188f, -0.571919f, -0.352371f),
       glm::vec3(0.996443f, -0.573179f, -0.358175f),
       glm::vec3(0.994151f, -0.575477f, -0.363965f),
       glm::vec3(0.991625f, -0.577494f, -0.369740f),
       glm::vec3(0.988492f, -0.579981f, -0.375502f),
       glm::vec3(0.985189f, -0.582474f, -0.381249f),
       glm::vec3(0.981306f, -0.585142f, -0.386981f),
       glm::vec3(0.977019f, -0.587904f, -0.392699f),
       glm::vec3(0.972414f, -0.590814f, -0.398401f),
       glm::vec3(0.967347f, -0.593846f, -0.404088f),
       glm::vec3(0.961988f, -0.597010f, -0.409759f),
       glm::vec3(0.956285f, -0.600281f, -0.415415f),
       glm::vec3(0.950162f, -0.603695f, -0.421055f),
       glm::vec3(0.943639f, -0.607418f, -0.426678f),
       glm::vec3(0.936728f, -0.611131f, -0.432285f),
       glm::vec3(0.929582f, -0.615093f, -0.437875f),
       glm::vec3(0.921973f, -0.619166f, -0.443448f),
       glm::vec3(0.913978f, -0.623463f, -0.449005f),
       glm::vec3(0.905765f, -0.627859f, -0.454544f),
       glm::vec3(0.897759f, -0.632398f, -0.460065f),
       glm::vec3(0.889585f, -0.637005f, -0.465569f),
       glm::vec3(0.881534f, -0.641634f, -0.471054f),
       glm::vec3(0.873080f, -0.646335f, -0.476522f),
       glm::vec3(0.865026f, -0.650799f, -0.481971f),
       glm::vec3(0.856699f, -0.655662f, -0.487402f),
       glm::vec3(0.848417f, -0.660321f, -0.492814f),
       glm::vec3(0.839973f, -0.665000f, -0.498207f),
       glm::vec3(0.831699f, -0.669738f, -0.503580f),
       glm::vec3(0.823276f, -0.674622f, -0.508935f),
       glm::vec3(0.814861f, -0.679435f, -0.514269f),
       glm::vec3(0.806457f, -0.684341f, -0.519584f),
       glm::vec3(0.797947f, -0.689249f, -0.524879f),
       glm::vec3(0.789474f, -0.694165f, -0.530153f),
       glm::vec3(0.780903f, -0.699108f, -0.535407f),
       glm::vec3(0.772385f, -0.704047f, -0.540641f),
       glm::vec3(0.763779f, -0.709000f, -0.545853f),
       glm::vec3(0.755184f, -0.713961f, -0.551045f),
       glm::vec3(0.746518f, -0.718951f, -0.556215f),
       glm::vec3(0.737922f, -0.723934f, -0.561364f),
       glm::vec3(0.729274f, -0.728963f, -0.566492f),
       glm::vec3(0.720685f, -0.733968f, -0.571597f),
       glm::vec3(0.712068f, -0.739021f, -0.576680f),
       glm::vec3(0.703537f, -0.744057f, -0.581741f),
       glm::vec3(0.694968f, -0.749125f, -0.586780f),
       glm::vec3(0.686482f, -0.754178f, -0.591796f),
       glm::vec3(0.677948f, -0.759258f, -0.596790f),
       glm::vec3(0.669515f, -0.764336f, -0.601760f),
       glm::vec3(0.661039f, -0.769424f, -0.606707f),
       glm::vec3(0.652670f, -0.774525f, -0.611631f),
       glm::vec3(0.644292f, -0.779625f, -0.616531f),
       glm::vec3(0.635999f, -0.784748f, -0.621407f),
       glm::vec3(0.627728f, -0.789839f, -0.626260f),
       glm::vec3(0.619496f, -0.794936f, -0.631088f),
       glm::vec3(0.611334f, -0.800029f, -0.635892f),
       glm::vec3(0.603219f, -0.805110f, -0.640671f),
       glm::vec3(0.595176f, -0.810173f, -0.645426f),
       glm::vec3(0.587156f, -0.815228f, -0.650156f),
       glm::vec3(0.579227f, -0.820272f, -0.654861f),
       glm::vec3(0.571330f, -0.825292f, -0.659540f),
       glm::vec3(0.563525f, -0.830333f, -0.664194f),
       glm::vec3(0.555752f, -0.835373f, -0.668823f),
       glm::vec3(0.548099f, -0.840366f, -0.673426f),
       glm::vec3(0.540486f, -0.845399f, -0.678002f),
       glm::vec3(0.532984f, -0.850385f, -0.682553f),
       glm::vec3(0.525580f, -0.855366f, -0.687077f),
       glm::vec3(0.518369f, -0.860519f, -0.691575f),
       glm::vec3(0.511164f, -0.865644f, -0.696047f),
       glm::vec3(0.504010f, -0.870735f, -0.700491f),
       glm::vec3(0.496990f, -0.875828f, -0.704908f),
       glm::vec3(0.490029f, -0.880925f, -0.709298f),
       glm::vec3(0.483180f, -0.885983f, -0.713661f),
       glm::vec3(0.476402f, -0.891066f, -0.717997f),
       glm::vec3(0.469728f, -0.896090f, -0.722304f),
       glm::vec3(0.463120f, -0.901140f, -0.726584f),
       glm::vec3(0.456631f, -0.906093f, -0.730836f),
       glm::vec3(0.450219f, -0.911037f, -0.735060f),
       glm::vec3(0.443889f, -0.915917f, -0.739255f),
       glm::vec3(0.437687f, -0.920746f, -0.743422f),
       glm::vec3(0.431570f, -0.925529f, -0.747560f),
       glm::vec3(0.425588f, -0.930251f, -0.751669f),
       glm::vec3(0.419707f, -0.934967f, -0.755750f),
       glm::vec3(0.413995f, -0.939573f, -0.759801f),
       glm::vec3(0.408358f, -0.944190f, -0.763823f),
       glm::vec3(0.402872f, -0.948679f, -0.767815f),
       glm::vec3(0.397439f, -0.953145f, -0.771778f),
       glm::vec3(0.392165f, -0.957480f, -0.775711f),
       glm::vec3(0.386915f, -0.961782f, -0.779615f),
       glm::vec3(0.381838f, -0.965894f, -0.783488f),
       glm::vec3(0.376750f, -0.969919f, -0.787331f),
       glm::vec3(0.371845f, -0.973732f, -0.791143f),
       glm::vec3(0.366884f, -0.977425f, -0.794926f),
       glm::vec3(0.362093f, -0.980828f, -0.798677f),
       glm::vec3(0.357148f, -0.984092f, -0.802398f),
       glm::vec3(0.352249f, -0.987294f, -0.806088f),
       glm::vec3(0.347463f, -0.990101f, -0.809746f),
       glm::vec3(0.343120f, -0.992242f, -0.813374f),
       glm::vec3(0.338398f, -0.994485f, -0.816970f),
       glm::vec3(0.333708f, -0.996509f, -0.820535f),
       glm::vec3(0.328732f, -0.998283f, -0.824068f),
       glm::vec3(0.324954f, -0.999027f, -0.827569f),
       glm::vec3(0.320256f, -0.999419f, -0.831038f),
       glm::vec3(0.315790f, -1.000000f, -0.834476f),
       glm::vec3(0.311315f, -0.999748f, -0.837881f),
       glm::vec3(0.307473f, -0.998900f, -0.841254f),
       glm::vec3(0.303432f, -0.997360f, -0.844594f),
       glm::vec3(0.300138f, -0.995556f, -0.847902f),
       glm::vec3(0.296491f, -0.992771f, -0.851177f),
       glm::vec3(0.293565f, -0.989742f, -0.854419f),
       glm::vec3(0.290597f, -0.986438f, -0.857629f),
       glm::vec3(0.287991f, -0.982895f, -0.860805f),
       glm::vec3(0.285476f, -0.978578f, -0.863948f),
       glm::vec3(0.283197f, -0.974585f, -0.867058f),
       glm::vec3(0.281104f, -0.970006f, -0.870135f),
       glm::vec3(0.279295f, -0.965158f, -0.873178f),
       glm::vec3(0.277690f, -0.960032f, -0.876187f),
       glm::vec3(0.276287f, -0.954750f, -0.879163f),
       glm::vec3(0.274927f, -0.949178f, -0.882104f),
       glm::vec3(0.273702f, -0.943601f, -0.885012f),
       glm::vec3(0.272500f, -0.937702f, -0.887885f),
       glm::vec3(0.271380f, -0.931770f, -0.890724f),
       glm::vec3(0.270261f, -0.925537f, -0.893529f),
       glm::vec3(0.269180f, -0.919276f, -0.896300f),
       glm::vec3(0.268068f, -0.912731f, -0.899036f),
       glm::vec3(0.266959f, -0.906173f, -0.901737f),
       glm::vec3(0.265792f, -0.899408f, -0.904404f),
       glm::vec3(0.264563f, -0.892601f, -0.907035f),
       glm::vec3(0.263274f, -0.885579f, -0.909632f),
       glm::vec3(0.261873f, -0.878557f, -0.912194f),
       glm::vec3(0.260459f, -0.871373f, -0.914720f),
       glm::vec3(0.258843f, -0.864077f, -0.917211f),
       glm::vec3(0.257159f, -0.856813f, -0.919667f),
       glm::vec3(0.255392f, -0.849287f, -0.922088f),
       glm::vec3(0.253834f, -0.841759f, -0.924472f),
       glm::vec3(0.251559f, -0.833784f, -0.926822f),
       glm::vec3(0.249781f, -0.826253f, -0.929135f),
       glm::vec3(0.247635f, -0.818111f, -0.931413f),
       glm::vec3(0.245521f, -0.810021f, -0.933655f),
       glm::vec3(0.243126f, -0.801766f, -0.935860f),
       glm::vec3(0.240714f, -0.793527f, -0.938030f),
       glm::vec3(0.238125f, -0.785092f, -0.940164f),
       glm::vec3(0.235421f, -0.776643f, -0.942261f),
       glm::vec3(0.232572f, -0.768027f, -0.944322f),
       glm::vec3(0.229562f, -0.759381f, -0.946346f),
       glm::vec3(0.226420f, -0.750595f, -0.948335f),
       glm::vec3(0.223110f, -0.741739f, -0.950286f),
       glm::vec3(0.219650f, -0.732765f, -0.952201f),
       glm::vec3(0.216046f, -0.723712f, -0.954079f),
       glm::vec3(0.212286f, -0.714559f, -0.955921f),
       glm::vec3(0.208402f, -0.705302f, -0.957725f),
       glm::vec3(0.204333f, -0.695947f, -0.959493f),
       glm::vec3(0.200144f, -0.686472f, -0.961224f),
       glm::vec3(0.195776f, -0.676917f, -0.962917f),
       glm::vec3(0.191275f, -0.667251f, -0.964574f),
       glm::vec3(0.186617f, -0.657509f, -0.966193f),
       glm::vec3(0.181811f, -0.647665f, -0.967775f),
       glm::vec3(0.176867f, -0.637866f, -0.969320f),
       glm::vec3(0.171770f, -0.628091f, -0.970827f),
       glm::vec3(0.166577f, -0.618502f, -0.972297f),
       glm::vec3(0.161243f, -0.609040f, -0.973730f),
       glm::vec3(0.155814f, -0.599779f, -0.975125f),
       glm::vec3(0.150284f, -0.590527f, -0.976482f),
       glm::vec3(0.144639f, -0.581187f, -0.977802f),
       glm::vec3(0.138878f, -0.571889f, -0.979084f),
       glm::vec3(0.132980f, -0.562497f, -0.980328f),
       glm::vec3(0.127004f, -0.553139f, -0.981535f),
       glm::vec3(0.120893f, -0.543674f, -0.982704f),
       glm::vec3(0.114727f, -0.534245f, -0.983834f),
       glm::vec3(0.108415f, -0.524705f, -0.984927f),
       glm::vec3(0.102074f, -0.515198f, -0.985982f),
       glm::vec3(0.095571f, -0.505566f, -0.986999f),
       glm::vec3(0.089048f, -0.495941f, -0.987978f),
       glm::vec3(0.082373f, -0.486235f, -0.988919f),
       glm::vec3(0.075670f, -0.476488f, -0.989821f),
       glm::vec3(0.068845f, -0.466684f, -0.990686f),
       glm::vec3(0.061955f, -0.456815f, -0.991512f),
       glm::vec3(0.054991f, -0.446898f, -0.992300f),
       glm::vec3(0.047942f, -0.436919f, -0.993050f),
       glm::vec3(0.040831f, -0.426881f, -0.993762f),
       glm::vec3(0.033625f, -0.416777f, -0.994435f),
       glm::vec3(0.026361f, -0.406591f, -0.995070f),
       glm::vec3(0.019005f, -0.396338f, -0.995667f),
       glm::vec3(0.011607f, -0.385995f, -0.996225f),
       glm::vec3(0.004114f, -0.375574f, -0.996744f),
       glm::vec3(-0.003430f, -0.365051f, -0.997226f),
       glm::vec3(-0.011088f, -0.354429f, -0.997669f),
       glm::vec3(-0.018810f, -0.343681f, -0.998073f),
       glm::vec3(-0.026636f, -0.332837f, -0.998439f),
       glm::vec3(-0.034510f, -0.321770f, -0.998767f),
       glm::vec3(-0.042553f, -0.310813f, -0.999056f),
       glm::vec3(-0.050641f, -0.299776f, -0.999306f),
       glm::vec3(-0.058637f, -0.288260f, -0.999518f),
       glm::vec3(-0.066656f, -0.277276f, -0.999692f),
       glm::vec3(-0.074752f, -0.266344f, -0.999827f),
       glm::vec3(-0.082708f, -0.255201f, -0.999923f),
       glm::vec3(-0.090693f, -0.244218f, -0.999981f),
       glm::vec3(-0.098679f, -0.233294f, -1.000000f),
       glm::vec3(-0.106634f, -0.222401f, -0.999981f),
       glm::vec3(-0.114597f, -0.211567f, -0.999923f),
       glm::vec3(-0.122515f, -0.200752f, -0.999827f),
       glm::vec3(-0.130447f, -0.189996f, -0.999692f),
       glm::vec3(-0.138352f, -0.179259f, -0.999518f),
       glm::vec3(-0.146276f, -0.168565f, -0.999306f),
       glm::vec3(-0.154179f, -0.157881f, -0.999056f),
       glm::vec3(-0.162098f, -0.147234f, -0.998767f),
       glm::vec3(-0.170003f, -0.136590f, -0.998439f),
       glm::vec3(-0.177916f, -0.125976f, -0.998073f),
       glm::vec3(-0.185831f, -0.115363f, -0.997669f),
       glm::vec3(-0.193749f, -0.104764f, -0.997226f),
       glm::vec3(-0.201686f, -0.094170f, -0.996744f),
       glm::vec3(-0.209634f, -0.083592f, -0.996225f),
       glm::vec3(-0.217609f, -0.073036f, -0.995667f),
       glm::vec3(-0.225580f, -0.062463f, -0.995070f),
       glm::vec3(-0.233586f, -0.051927f, -0.994435f),
       glm::vec3(-0.241598f, -0.041345f, -0.993762f),
       glm::vec3(-0.249636f, -0.030829f, -0.993050f),
       glm::vec3(-0.257679f, -0.020265f, -0.992300f),
       glm::vec3(-0.265783f, -0.009728f, -0.991512f),
       glm::vec3(-0.273904f, 0.000842f, -0.990686f),
       glm::vec3(-0.282088f, 0.011412f, -0.989821f),
       glm::vec3(-0.290295f, 0.021990f, -0.988919f),
       glm::vec3(-0.298577f, 0.032596f, -0.987978f),
       glm::vec3(-0.306915f, 0.043198f, -0.986999f),
       glm::vec3(-0.315303f, 0.053818f, -0.985982f),
       glm::vec3(-0.323763f, 0.064437f, -0.984927f),
       glm::vec3(-0.332297f, 0.075104f, -0.983834f),
       glm::vec3(-0.340911f, 0.085756f, -0.982704f),
       glm::vec3(-0.349633f, 0.096460f, -0.981535f),
       glm::vec3(-0.358422f, 0.107157f, -0.980328f),
       glm::vec3(-0.367197f, 0.117868f, -0.979084f),
       glm::vec3(-0.376081f, 0.128462f, -0.977802f),
       glm::vec3(-0.384875f, 0.138854f, -0.976482f),
       glm::vec3(-0.393635f, 0.149047f, -0.975125f),
       glm::vec3(-0.402394f, 0.159071f, -0.973730f),
       glm::vec3(-0.411135f, 0.168929f, -0.972297f),
       glm::vec3(-0.420001f, 0.178577f, -0.970827f),
       glm::vec3(-0.428790f, 0.188027f, -0.969320f),
       glm::vec3(-0.437645f, 0.197404f, -0.967775f),
       glm::vec3(-0.446558f, 0.206497f, -0.966193f),
       glm::vec3(-0.455498f, 0.215433f, -0.964574f),
       glm::vec3(-0.464499f, 0.224218f, -0.962917f),
       glm::vec3(-0.473537f, 0.232802f, -0.961224f),
       glm::vec3(-0.482599f, 0.241208f, -0.959493f),
       glm::vec3(-0.491751f, 0.249418f, -0.957725f),
       glm::vec3(-0.500914f, 0.257438f, -0.955921f),
       glm::vec3(-0.510151f, 0.265248f, -0.954079f),
       glm::vec3(-0.519473f, 0.272884f, -0.952201f),
       glm::vec3(-0.528710f, 0.280173f, -0.950286f),
       glm::vec3(-0.537846f, 0.287117f, -0.948335f),
       glm::vec3(-0.546953f, 0.293713f, -0.946346f),
       glm::vec3(-0.555997f, 0.299968f, -0.944322f),
       glm::vec3(-0.565000f, 0.305887f, -0.942261f),
       glm::vec3(-0.573988f, 0.311449f, -0.940164f),
       glm::vec3(-0.582888f, 0.316635f, -0.938030f),
       glm::vec3(-0.591838f, 0.321481f, -0.935860f),
       glm::vec3(-0.600704f, 0.325920f, -0.933655f),
       glm::vec3(-0.609612f, 0.330020f, -0.931413f),
       glm::vec3(-0.618465f, 0.333713f, -0.929135f),
       glm::vec3(-0.627333f, 0.337061f, -0.926822f),
       glm::vec3(-0.636169f, 0.340026f, -0.924472f),
       glm::vec3(-0.645003f, 0.342613f, -0.922088f),
       glm::vec3(-0.653800f, 0.344849f, -0.919667f),
       glm::vec3(-0.662607f, 0.346698f, -0.917211f),
       glm::vec3(-0.671388f, 0.348196f, -0.914720f),
       glm::vec3(-0.680158f, 0.349303f, -0.912194f),
       glm::vec3(-0.688912f, 0.350082f, -0.909632f),
       glm::vec3(-0.697640f, 0.350471f, -0.907035f),
       glm::vec3(-0.706370f, 0.350524f, -0.904404f),
       glm::vec3(-0.715069f, 0.350207f, -0.901737f),
       glm::vec3(-0.723762f, 0.349526f, -0.899036f),
       glm::vec3(-0.732415f, 0.348517f, -0.896300f),
       glm::vec3(-0.741095f, 0.347128f, -0.893529f),
       glm::vec3(-0.749696f, 0.345428f, -0.890724f),
       glm::vec3(-0.758325f, 0.343345f, -0.887885f),
       glm::vec3(-0.766851f, 0.340969f, -0.885012f),
       glm::vec3(-0.775414f, 0.338227f, -0.882104f),
       glm::vec3(-0.783856f, 0.335202f, -0.879163f),
       glm::vec3(-0.792334f, 0.331829f, -0.876187f),
       glm::vec3(-0.800696f, 0.328171f, -0.873178f),
       glm::vec3(-0.809097f, 0.324193f, -0.870135f),
       glm::vec3(-0.817376f, 0.319951f, -0.867058f),
       glm::vec3(-0.825677f, 0.315404f, -0.863948f),
       glm::vec3(-0.833874f, 0.310591f, -0.860805f),
       glm::vec3(-0.842119f, 0.305468f, -0.857629f),
       glm::vec3(-0.850252f, 0.300093f, -0.854419f),
       glm::vec3(-0.858415f, 0.294434f, -0.851177f),
       glm::vec3(-0.866445f, 0.288527f, -0.847902f),
       glm::vec3(-0.874539f, 0.282521f, -0.844594f),
       glm::vec3(-0.882349f, 0.276260f, -0.841254f),
       glm::vec3(-0.889872f, 0.269847f, -0.837881f),
       glm::vec3(-0.897183f, 0.263324f, -0.834476f),
       glm::vec3(-0.904234f, 0.256617f, -0.831038f),
       glm::vec3(-0.911049f, 0.249802f, -0.827569f),
       glm::vec3(-0.917588f, 0.242823f, -0.824068f),
       glm::vec3(-0.923910f, 0.235741f, -0.820535f),
       glm::vec3(-0.929950f, 0.228540f, -0.816970f),
       glm::vec3(-0.935768f, 0.221246f, -0.813374f),
       glm::vec3(-0.941331f, 0.213862f, -0.809746f),
       glm::vec3(-0.946655f, 0.206388f, -0.806088f),
       glm::vec3(-0.951764f, 0.198865f, -0.802398f),
       glm::vec3(-0.956622f, 0.191233f, -0.798677f),
       glm::vec3(-0.961243f, 0.183611f, -0.794926f),
       glm::vec3(-0.965622f, 0.175878f, -0.791143f),
       glm::vec3(-0.969753f, 0.168187f, -0.787331f),
       glm::vec3(-0.973655f, 0.160386f, -0.783488f),
       glm::vec3(-0.977289f, 0.152642f, -0.779615f),
       glm::vec3(-0.980674f, 0.144812f, -0.775711f),
       glm::vec3(-0.983770f, 0.137047f, -0.771778f),
       glm::vec3(-0.986622f, 0.129224f, -0.767815f),
       glm::vec3(-0.989163f, 0.121473f, -0.763823f),
       glm::vec3(-0.991477f, 0.113693f, -0.759801f),
       glm::vec3(-0.993461f, 0.106005f, -0.755750f),
       glm::vec3(-0.995228f, 0.098302f, -0.751669f),
       glm::vec3(-0.996665f, 0.090713f, -0.747560f),
       glm::vec3(-0.997896f, 0.083130f, -0.743422f),
       glm::vec3(-0.998806f, 0.075694f, -0.739255f),
       glm::vec3(-0.999491f, 0.068265f, -0.735060f),
       glm::vec3(-0.999870f, 0.061019f, -0.730836f),
       glm::vec3(-1.000000f, 0.053789f, -0.726584f),
       glm::vec3(-0.999833f, 0.046793f, -0.722304f),
       glm::vec3(-0.999385f, 0.039824f, -0.717997f),
       glm::vec3(-0.998658f, 0.033100f, -0.713661f),
       glm::vec3(-0.997620f, 0.026430f, -0.709298f),
       glm::vec3(-0.996297f, 0.020041f, -0.704908f),
       glm::vec3(-0.994620f, 0.013737f, -0.700491f),
       glm::vec3(-0.992668f, 0.007752f, -0.696047f),
       glm::vec3(-0.990346f, 0.001892f, -0.691575f),
       glm::vec3(-0.987784f, -0.003626f, -0.687077f),
       glm::vec3(-0.984845f, -0.008991f, -0.682553f),
       glm::vec3(-0.981672f, -0.013983f, -0.678002f),
       glm::vec3(-0.978130f, -0.018755f, -0.673426f),
       glm::vec3(-0.974361f, -0.023141f, -0.668823f),
       glm::vec3(-0.970212f, -0.027276f, -0.664194f),
       glm::vec3(-0.965861f, -0.031003f, -0.659540f),
       glm::vec3(-0.961147f, -0.034429f, -0.654861f),
       glm::vec3(-0.956261f, -0.037425f, -0.650156f),
       glm::vec3(-0.950990f, -0.040113f, -0.645426f),
       glm::vec3(-0.945602f, -0.042338f, -0.640671f),
       glm::vec3(-0.939856f, -0.044227f, -0.635892f),
       glm::vec3(-0.934014f, -0.045666f, -0.631088f),
       glm::vec3(-0.927844f, -0.046739f, -0.626260f),
       glm::vec3(-0.921660f, -0.047366f, -0.621407f),
       glm::vec3(-0.915169f, -0.047595f, -0.616531f),
       glm::vec3(-0.908659f, -0.047395f, -0.611631f),
       glm::vec3(-0.901902f, -0.046768f, -0.606707f),
       glm::vec3(-0.895135f, -0.045741f, -0.601760f),
       glm::vec3(-0.888190f, -0.044282f, -0.596790f),
       glm::vec3(-0.881242f, -0.042427f, -0.591796f),
       glm::vec3(-0.874151f, -0.040141f, -0.586780f),
       glm::vec3(-0.867055f, -0.037547f, -0.581741f),
       glm::vec3(-0.859734f, -0.034554f, -0.576680f),
       glm::vec3(-0.852441f, -0.031272f, -0.571597f),
       glm::vec3(-0.845005f, -0.027572f, -0.566492f),
       glm::vec3(-0.837527f, -0.023636f, -0.561364f),
       glm::vec3(-0.829905f, -0.019324f, -0.556215f),
       glm::vec3(-0.822223f, -0.014686f, -0.551045f),
       glm::vec3(-0.814477f, -0.009711f, -0.545853f),
       glm::vec3(-0.806656f, -0.004407f, -0.540641f),
       glm::vec3(-0.798738f, 0.001211f, -0.535407f),
       glm::vec3(-0.790729f, 0.007151f, -0.530153f),
       glm::vec3(-0.782630f, 0.013387f, -0.524879f),
       glm::vec3(-0.774436f, 0.019956f, -0.519584f),
       glm::vec3(-0.766130f, 0.026792f, -0.514269f),
       glm::vec3(-0.757726f, 0.033900f, -0.508935f),
       glm::vec3(-0.749218f, 0.041312f, -0.503580f),
       glm::vec3(-0.740538f, 0.049063f, -0.498207f),
       glm::vec3(-0.731789f, 0.057069f, -0.492814f),
       glm::vec3(-0.722929f, 0.065309f, -0.487402f),
       glm::vec3(-0.714039f, 0.073852f, -0.481971f),
       glm::vec3(-0.704606f, 0.082590f, -0.476522f),
       glm::vec3(-0.695319f, 0.091414f, -0.471054f),
       glm::vec3(-0.686055f, 0.100198f, -0.465569f),
       glm::vec3(-0.676834f, 0.109117f, -0.460065f),
       glm::vec3(-0.667560f, 0.118069f, -0.454544f),
       glm::vec3(-0.658333f, 0.127119f, -0.449005f),
       glm::vec3(-0.649072f, 0.136187f, -0.443448f),
       glm::vec3(-0.639804f, 0.145324f, -0.437875f),
       glm::vec3(-0.630466f, 0.154511f, -0.432285f),
       glm::vec3(-0.621237f, 0.163758f, -0.426678f),
       glm::vec3(-0.611875f, 0.173064f, -0.421055f),
       glm::vec3(-0.602532f, 0.182311f, -0.415415f),
       glm::vec3(-0.593125f, 0.191677f, -0.409759f),
       glm::vec3(-0.583848f, 0.201003f, -0.404088f),
       glm::vec3(-0.574250f, 0.210471f, -0.398401f),
       glm::vec3(-0.564849f, 0.219746f, -0.392699f),
       glm::vec3(-0.555299f, 0.229258f, -0.386981f),
       glm::vec3(-0.545765f, 0.238658f, -0.381249f),
       glm::vec3(-0.536103f, 0.248148f, -0.375502f),
       glm::vec3(-0.526548f, 0.257558f, -0.369740f),
       glm::vec3(-0.516857f, 0.267142f, -0.363965f),
       glm::vec3(-0.507140f, 0.276669f, -0.358175f),
       glm::vec3(-0.497648f, 0.286049f, -0.352371f),
       glm::vec3(-0.488170f, 0.295177f, -0.346554f),
       glm::vec3(-0.478981f, 0.304210f, -0.340723f),
       glm::vec3(-0.469709f, 0.313169f, -0.334880f),
       glm::vec3(-0.460652f, 0.321810f, -0.329023f),
       glm::vec3(-0.451689f, 0.330381f, -0.323154f),
       glm::vec3(-0.442871f, 0.338741f, -0.317272f),
       glm::vec3(-0.434095f, 0.347010f, -0.311378f),
       glm::vec3(-0.425439f, 0.355060f, -0.305472f),
       glm::vec3(-0.416868f, 0.363003f, -0.299554f),
       glm::vec3(-0.408384f, 0.370743f, -0.293625f),
       glm::vec3(-0.400002f, 0.378342f, -0.287684f),
       glm::vec3(-0.391696f, 0.385775f, -0.281733f),
       glm::vec3(-0.383477f, 0.393039f, -0.275770f),
       glm::vec3(-0.375347f, 0.400156f, -0.269797f),
       glm::vec3(-0.367279f, 0.407095f, -0.263813f),
       glm::vec3(-0.359310f, 0.413897f, -0.257819f),
       glm::vec3(-0.351405f, 0.420515f, -0.251816f),
       glm::vec3(-0.343608f, 0.426993f, -0.245802f),
       glm::vec3(-0.335881f, 0.433281f, -0.239779f),
       glm::vec3(-0.328254f, 0.439435f, -0.233747f),
       glm::vec3(-0.320707f, 0.445406f, -0.227706f),
       glm::vec3(-0.313250f, 0.451233f, -0.221656f),
       glm::vec3(-0.305896f, 0.456892f, -0.215598f),
       glm::vec3(-0.298628f, 0.462377f, -0.209531f),
       glm::vec3(-0.291486f, 0.467719f, -0.203456f),
       glm::vec3(-0.284403f, 0.472909f, -0.197373f),
       glm::vec3(-0.277474f, 0.477873f, -0.191283f),
       glm::vec3(-0.270556f, 0.482691f, -0.185185f),
       glm::vec3(-0.263823f, 0.487285f, -0.179080f),
       glm::vec3(-0.257125f, 0.491772f, -0.172969f),
       glm::vec3(-0.250594f, 0.495986f, -0.166850f),
       glm::vec3(-0.244161f, 0.500039f, -0.160726f),
       glm::vec3(-0.237876f, 0.503909f, -0.154595f),
       glm::vec3(-0.231687f, 0.507624f, -0.148458f),
       glm::vec3(-0.225679f, 0.511155f, -0.142315f),
       glm::vec3(-0.219750f, 0.514479f, -0.136167f),
       glm::vec3(-0.214012f, 0.517616f, -0.130013f),
       glm::vec3(-0.208372f, 0.520540f, -0.123855f),
       glm::vec3(-0.202842f, 0.523290f, -0.117692f),
       glm::vec3(-0.197480f, 0.525795f, -0.111524f),
       glm::vec3(-0.192196f, 0.528154f, -0.105352f),
       glm::vec3(-0.187142f, 0.530235f, -0.099176f),
       glm::vec3(-0.182096f, 0.532238f, -0.092996f),
       glm::vec3(-0.177402f, 0.533993f, -0.086812f),
       glm::vec3(-0.172739f, 0.535600f, -0.080625f),
       glm::vec3(-0.168713f, 0.536960f, -0.074435f),
       glm::vec3(-0.164399f, 0.538182f, -0.068242f),
       glm::vec3(-0.160393f, 0.539178f, -0.062047f),
       glm::vec3(-0.156885f, 0.539969f, -0.055849f),
       glm::vec3(-0.153752f, 0.540428f, -0.049649f),
       glm::vec3(-0.149974f, 0.540668f, -0.043447f),
       glm::vec3(-0.147462f, 0.540700f, -0.037243f),
       glm::vec3(-0.145638f, 0.540273f, -0.031038f),
       glm::vec3(-0.143353f, 0.539772f, -0.024832f),
       glm::vec3(-0.142656f, 0.539233f, -0.018625f),
       glm::vec3(-0.141939f, 0.538908f, -0.012417f),
       glm::vec3(-0.141254f, 0.538741f, -0.006209f),
       glm::vec3(-0.140899f, 0.538806f, -0.000000f),
       //Chromosome 3
       glm::vec3(0.447380f, 0.354913f, -0.940358f),
       glm::vec3(-0.585366f, -0.629411f, -0.938647f),
       glm::vec3(-0.585222f, -0.629279f, -0.936913f),
       glm::vec3(-0.585843f, -0.630437f, -0.935155f),
       glm::vec3(-0.585100f, -0.630849f, -0.933374f),
       glm::vec3(-0.584334f, -0.633451f, -0.931570f),
       glm::vec3(-0.583148f, -0.634724f, -0.929743f),
       glm::vec3(-0.581089f, -0.637040f, -0.927892f),
       glm::vec3(-0.579150f, -0.639133f, -0.926018f),
       glm::vec3(-0.576545f, -0.641539f, -0.924121f),
       glm::vec3(-0.573985f, -0.643929f, -0.922201f),
       glm::vec3(-0.570913f, -0.646472f, -0.920258f),
       glm::vec3(-0.567624f, -0.649107f, -0.918292f),
       glm::vec3(-0.564088f, -0.651762f, -0.916303f),
       glm::vec3(-0.560326f, -0.654559f, -0.914292f),
       glm::vec3(-0.556347f, -0.657274f, -0.912257f),
       glm::vec3(-0.552078f, -0.660126f, -0.910200f),
       glm::vec3(-0.547616f, -0.662916f, -0.908120f),
       glm::vec3(-0.542847f, -0.665813f, -0.906017f),
       glm::vec3(-0.537981f, -0.668675f, -0.903892f),
       glm::vec3(-0.532771f, -0.671560f, -0.901744f),
       glm::vec3(-0.527447f, -0.674427f, -0.899574f),
       glm::vec3(-0.521810f, -0.677307f, -0.897381f),
       glm::vec3(-0.516080f, -0.680124f, -0.895166f),
       glm::vec3(-0.510074f, -0.682974f, -0.892929f),
       glm::vec3(-0.503949f, -0.685756f, -0.890669f),
       glm::vec3(-0.497553f, -0.688563f, -0.888387f),
       glm::vec3(-0.491045f, -0.691293f, -0.886083f),
       glm::vec3(-0.484286f, -0.694015f, -0.883757f),
       glm::vec3(-0.477394f, -0.696678f, -0.881408f),
       glm::vec3(-0.470277f, -0.699296f, -0.879038f),
       glm::vec3(-0.463024f, -0.701853f, -0.876646f),
       glm::vec3(-0.455570f, -0.704319f, -0.874232f),
       glm::vec3(-0.447951f, -0.706727f, -0.871796f),
       glm::vec3(-0.440149f, -0.709022f, -0.869339f),
       glm::vec3(-0.432161f, -0.711251f, -0.866859f),
       glm::vec3(-0.424054f, -0.713342f, -0.864358f),
       glm::vec3(-0.415722f, -0.715378f, -0.861836f),
       glm::vec3(-0.407252f, -0.717284f, -0.859292f),
       glm::vec3(-0.398604f, -0.719079f, -0.856727f),
       glm::vec3(-0.389825f, -0.720770f, -0.854140f),
       glm::vec3(-0.380835f, -0.722302f, -0.851532f),
       glm::vec3(-0.371755f, -0.723735f, -0.848902f),
       glm::vec3(-0.362443f, -0.724987f, -0.846252f),
       glm::vec3(-0.353055f, -0.726123f, -0.843580f),
       glm::vec3(-0.343428f, -0.727079f, -0.840888f),
       glm::vec3(-0.333729f, -0.727912f, -0.838174f),
       glm::vec3(-0.323800f, -0.728558f, -0.835440f),
       glm::vec3(-0.313788f, -0.729085f, -0.832684f),
       glm::vec3(-0.303573f, -0.729412f, -0.829908f),
       glm::vec3(-0.293253f, -0.729619f, -0.827111f),
       glm::vec3(-0.282767f, -0.729626f, -0.824294f),
       glm::vec3(-0.272150f, -0.729462f, -0.821456f),
       glm::vec3(-0.261388f, -0.729125f, -0.818597f),
       glm::vec3(-0.250493f, -0.728581f, -0.815718f),
       glm::vec3(-0.239468f, -0.727865f, -0.812819f),
       glm::vec3(-0.228326f, -0.726919f, -0.809899f),
       glm::vec3(-0.217052f, -0.725791f, -0.806960f),
       glm::vec3(-0.205657f, -0.724436f, -0.804000f),
       glm::vec3(-0.194133f, -0.722896f, -0.801020f),
       glm::vec3(-0.182497f, -0.721130f, -0.798020f),
       glm::vec3(-0.170728f, -0.719158f, -0.795000f),
       glm::vec3(-0.158847f, -0.716980f, -0.791960f),
       glm::vec3(-0.146834f, -0.714582f, -0.788901f),
       glm::vec3(-0.134700f, -0.711982f, -0.785822f),
       glm::vec3(-0.122508f, -0.709085f, -0.782723f),
       glm::vec3(-0.110160f, -0.706038f, -0.779605f),
       glm::vec3(-0.097712f, -0.702802f, -0.776467f),
       glm::vec3(-0.085007f, -0.699278f, -0.773310f),
       glm::vec3(-0.072367f, -0.695580f, -0.770133f),
       glm::vec3(-0.059452f, -0.691730f, -0.766938f),
       glm::vec3(-0.046638f, -0.687625f, -0.763723f),
       glm::vec3(-0.033355f, -0.683198f, -0.760489f),
       glm::vec3(-0.020173f, -0.678747f, -0.757237f),
       glm::vec3(-0.006946f, -0.673943f, -0.753965f),
       glm::vec3(0.006481f, -0.668927f, -0.750675f),
       glm::vec3(0.019980f, -0.663715f, -0.747365f),
       glm::vec3(0.033597f, -0.658277f, -0.744038f),
       glm::vec3(0.047300f, -0.652643f, -0.740691f),
       glm::vec3(0.061109f, -0.646796f, -0.737326f),
       glm::vec3(0.075006f, -0.640758f, -0.733943f),
       glm::vec3(0.088999f, -0.634509f, -0.730542f),
       glm::vec3(0.103071f, -0.628076f, -0.727122f),
       glm::vec3(0.117207f, -0.621436f, -0.723684f),
       glm::vec3(0.131414f, -0.614619f, -0.720228f),
       glm::vec3(0.145668f, -0.607585f, -0.716754f),
       glm::vec3(0.159954f, -0.600358f, -0.713262f),
       glm::vec3(0.174088f, -0.592920f, -0.709753f),
       glm::vec3(0.188139f, -0.585389f, -0.706226f),
       glm::vec3(0.202065f, -0.577736f, -0.702681f),
       glm::vec3(0.215878f, -0.569995f, -0.699118f),
       glm::vec3(0.229551f, -0.562170f, -0.695538f),
       glm::vec3(0.243105f, -0.554282f, -0.691941f),
       glm::vec3(0.256527f, -0.546322f, -0.688327f),
       glm::vec3(0.269822f, -0.538297f, -0.684695f),
       glm::vec3(0.282952f, -0.530230f, -0.681046f),
       glm::vec3(0.295948f, -0.522100f, -0.677380f),
       glm::vec3(0.308781f, -0.513930f, -0.673698f),
       glm::vec3(0.321469f, -0.505703f, -0.669998f),
       glm::vec3(0.333963f, -0.497446f, -0.666282f),
       glm::vec3(0.346301f, -0.489139f, -0.662549f),
       glm::vec3(0.358419f, -0.480815f, -0.658800f),
       glm::vec3(0.370396f, -0.472418f, -0.655034f),
       glm::vec3(0.382114f, -0.464046f, -0.651252f),
       glm::vec3(0.393566f, -0.455614f, -0.647454f),
       glm::vec3(0.404962f, -0.447210f, -0.643639f),
       glm::vec3(0.415971f, -0.438816f, -0.639809f),
       glm::vec3(0.426808f, -0.430491f, -0.635962f),
       glm::vec3(0.437669f, -0.422359f, -0.632100f),
       glm::vec3(0.448383f, -0.414402f, -0.628222f),
       glm::vec3(0.459034f, -0.406594f, -0.624328f),
       glm::vec3(0.469574f, -0.398975f, -0.620419f),
       glm::vec3(0.480044f, -0.391499f, -0.616494f),
       glm::vec3(0.490340f, -0.384253f, -0.612554f),
       glm::vec3(0.500524f, -0.377181f, -0.608598f),
       glm::vec3(0.510607f, -0.370007f, -0.604628f),
       glm::vec3(0.520346f, -0.362836f, -0.600642f),
       glm::vec3(0.529904f, -0.355634f, -0.596641f),
       glm::vec3(0.539199f, -0.348415f, -0.592625f),
       glm::vec3(0.548248f, -0.341214f, -0.588595f),
       glm::vec3(0.557020f, -0.334010f, -0.584550f),
       glm::vec3(0.565494f, -0.326818f, -0.580490f),
       glm::vec3(0.573688f, -0.319639f, -0.576416f),
       glm::vec3(0.581551f, -0.312467f, -0.572327f),
       glm::vec3(0.589101f, -0.305341f, -0.568225f),
       glm::vec3(0.596292f, -0.298209f, -0.564108f),
       glm::vec3(0.603155f, -0.291122f, -0.559977f),
       glm::vec3(0.609649f, -0.284033f, -0.555831f),
       glm::vec3(0.615788f, -0.276994f, -0.551673f),
       glm::vec3(0.621518f, -0.269988f, -0.547500f),
       glm::vec3(0.626874f, -0.263011f, -0.543314f),
       glm::vec3(0.631800f, -0.256103f, -0.539114f),
       glm::vec3(0.636313f, -0.249214f, -0.534900f),
       glm::vec3(0.640355f, -0.242433f, -0.530674f),
       glm::vec3(0.643939f, -0.235632f, -0.526434f),
       glm::vec3(0.647030f, -0.228984f, -0.522181f),
       glm::vec3(0.649619f, -0.222305f, -0.517915f),
       glm::vec3(0.651693f, -0.215798f, -0.513636f),
       glm::vec3(0.653208f, -0.209309f, -0.509344f),
       glm::vec3(0.654184f, -0.203020f, -0.505039f),
       glm::vec3(0.654574f, -0.196784f, -0.500722f),
       glm::vec3(0.654399f, -0.190749f, -0.496393f),
       glm::vec3(0.653656f, -0.184790f, -0.492051f),
       glm::vec3(0.652290f, -0.179103f, -0.487696f),
       glm::vec3(0.650351f, -0.173518f, -0.483330f),
       glm::vec3(0.647839f, -0.168209f, -0.478952f),
       glm::vec3(0.644748f, -0.163104f, -0.474561f),
       glm::vec3(0.641095f, -0.158258f, -0.470159f),
       glm::vec3(0.636944f, -0.153708f, -0.465745f),
       glm::vec3(0.632217f, -0.149405f, -0.461319f),
       glm::vec3(0.627046f, -0.145427f, -0.456882f),
       glm::vec3(0.621322f, -0.141740f, -0.452434f),
       glm::vec3(0.615180f, -0.138375f, -0.447974f),
       glm::vec3(0.608572f, -0.135307f, -0.443503f),
       glm::vec3(0.601566f, -0.132599f, -0.439021f),
       glm::vec3(0.594115f, -0.130155f, -0.434528f),
       glm::vec3(0.586342f, -0.128061f, -0.430024f),
       glm::vec3(0.578137f, -0.126189f, -0.425509f),
       glm::vec3(0.569672f, -0.124657f, -0.420984f),
       glm::vec3(0.560779f, -0.123366f, -0.416449f),
       glm::vec3(0.551672f, -0.122354f, -0.411903f),
       glm::vec3(0.542199f, -0.121576f, -0.407346f),
       glm::vec3(0.532511f, -0.121066f, -0.402780f),
       glm::vec3(0.522505f, -0.120815f, -0.398203f),
       glm::vec3(0.512310f, -0.120831f, -0.393617f),
       glm::vec3(0.501803f, -0.121072f, -0.389020f),
       glm::vec3(0.491120f, -0.121558f, -0.384414f),
       glm::vec3(0.480153f, -0.122283f, -0.379799f),
       glm::vec3(0.469062f, -0.123238f, -0.375174f),
       glm::vec3(0.457700f, -0.124433f, -0.370539f),
       glm::vec3(0.446242f, -0.125846f, -0.365896f),
       glm::vec3(0.434536f, -0.127491f, -0.361243f),
       glm::vec3(0.422760f, -0.129343f, -0.356581f),
       glm::vec3(0.410751f, -0.131419f, -0.351910f),
       glm::vec3(0.398699f, -0.133685f, -0.347231f),
       glm::vec3(0.386409f, -0.136172f, -0.342543f),
       glm::vec3(0.374116f, -0.138837f, -0.337846f),
       glm::vec3(0.361613f, -0.141722f, -0.333141f),
       glm::vec3(0.349113f, -0.144769f, -0.328427f),
       glm::vec3(0.336427f, -0.148046f, -0.323706f),
       glm::vec3(0.323724f, -0.151481f, -0.318976f),
       glm::vec3(0.310898f, -0.155128f, -0.314238f),
       glm::vec3(0.298054f, -0.158933f, -0.309493f),
       glm::vec3(0.285123f, -0.162924f, -0.304740f),
       glm::vec3(0.272165f, -0.167078f, -0.299979f),
       glm::vec3(0.259143f, -0.171400f, -0.295211f),
       glm::vec3(0.246117f, -0.175875f, -0.290435f),
       glm::vec3(0.233029f, -0.180500f, -0.285652f),
       glm::vec3(0.219924f, -0.185272f, -0.280862f),
       glm::vec3(0.206780f, -0.190174f, -0.276065f),
       glm::vec3(0.193618f, -0.195222f, -0.271261f),
       glm::vec3(0.180408f, -0.200389f, -0.266450f),
       glm::vec3(0.167194f, -0.205684f, -0.261633f),
       glm::vec3(0.153951f, -0.211080f, -0.256809f),
       glm::vec3(0.140717f, -0.216594f, -0.251979f),
       glm::vec3(0.127461f, -0.222197f, -0.247142f),
       glm::vec3(0.114232f, -0.227902f, -0.242300f),
       glm::vec3(0.100992f, -0.233682f, -0.237451f),
       glm::vec3(0.087774f, -0.239558f, -0.232596f),
       glm::vec3(0.074558f, -0.245495f, -0.227736f),
       glm::vec3(0.061372f, -0.251518f, -0.222869f),
       glm::vec3(0.048205f, -0.257583f, -0.217998f),
       glm::vec3(0.035080f, -0.263726f, -0.213121f),
       glm::vec3(0.021981f, -0.269894f, -0.208238f),
       glm::vec3(0.008922f, -0.276185f, -0.203350f),
       glm::vec3(-0.004124f, -0.282394f, -0.198458f),
       glm::vec3(-0.017129f, -0.288595f, -0.193560f),
       glm::vec3(-0.030005f, -0.295102f, -0.188657f),
       glm::vec3(-0.042827f, -0.301431f, -0.183750f),
       glm::vec3(-0.055705f, -0.307656f, -0.178838f),
       glm::vec3(-0.068176f, -0.314032f, -0.173922f),
       glm::vec3(-0.080433f, -0.320180f, -0.169001f),
       glm::vec3(-0.092462f, -0.326219f, -0.164077f),
       glm::vec3(-0.104201f, -0.332133f, -0.159148f),
       glm::vec3(-0.115698f, -0.337932f, -0.154215f),
       glm::vec3(-0.126905f, -0.343609f, -0.149278f),
       glm::vec3(-0.137867f, -0.349151f, -0.144337f),
       glm::vec3(-0.148540f, -0.354571f, -0.139393f),
       glm::vec3(-0.158949f, -0.359849f, -0.134446f),
       glm::vec3(-0.169071f, -0.364979f, -0.129495f),
       glm::vec3(-0.178938f, -0.369965f, -0.124541f),
       glm::vec3(-0.188494f, -0.374811f, -0.119584f),
       glm::vec3(-0.197764f, -0.379504f, -0.114623f),
       glm::vec3(-0.206716f, -0.384046f, -0.109660f),
       glm::vec3(-0.215423f, -0.388455f, -0.104694f),
       glm::vec3(-0.223769f, -0.392685f, -0.099726f),
       glm::vec3(-0.231822f, -0.396792f, -0.094755f),
       glm::vec3(-0.239561f, -0.400702f, -0.089782f),
       glm::vec3(-0.246986f, -0.404493f, -0.084806f),
       glm::vec3(-0.254079f, -0.408068f, -0.079829f),
       glm::vec3(-0.260855f, -0.411494f, -0.074849f),
       glm::vec3(-0.267330f, -0.414736f, -0.069867f),
       glm::vec3(-0.273442f, -0.417808f, -0.064884f),
       glm::vec3(-0.279313f, -0.420735f, -0.059899f),
       glm::vec3(-0.284836f, -0.423457f, -0.054913f),
       glm::vec3(-0.290054f, -0.425965f, -0.049925f),
       glm::vec3(-0.294664f, -0.428153f, -0.044936f),
       glm::vec3(-0.299214f, -0.430279f, -0.039946f),
       glm::vec3(-0.303206f, -0.432021f, -0.034955f),
       glm::vec3(-0.306997f, -0.433788f, -0.029963f),
       glm::vec3(-0.309775f, -0.435038f, -0.024970f),
       glm::vec3(-0.312823f, -0.436400f, -0.019977f),
       glm::vec3(-0.314836f, -0.437114f, -0.014983f),
       glm::vec3(-0.316038f, -0.438417f, -0.009989f),
       glm::vec3(-0.317471f, -0.438297f, -0.004995f),
       glm::vec3(-0.318131f, -0.439382f, -0.000000f),
};
    // Define your scaling factor
    float scaleFactor = 10.0f; // Adjust this value based on your needs

    // Scale each position in the instancePositions vector
    for (glm::vec3& position : instancePositions) {
        position *= scaleFactor;
    }

    GLuint instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3), &instancePositions[0], GL_STATIC_DRAW);

    // Assuming you have a VAO already bound
    glEnableVertexAttribArray(1); // Attribute 1 for instance positions
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(1, 1); // Tell OpenGL this is an instanced vertex attribute.

    //Creating Chromosome IDs//
    std::vector<float> chromosomeIDs;
    for (int i = 0; i < 1258; ++i) {
        float id = (i < 558) ? 1.0f : ((i < 1012) ? 2.0f : 3.0f);
        chromosomeIDs.push_back(id);
    }

    GLuint chromoIDVBO;
    glGenBuffers(1, &chromoIDVBO);
    glBindBuffer(GL_ARRAY_BUFFER, chromoIDVBO);
    glBufferData(GL_ARRAY_BUFFER, chromosomeIDs.size() * sizeof(float), chromosomeIDs.data(), GL_STATIC_DRAW);


    /* CREATE & BIND VERTEX-ARRAY OBJECT */
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    
    /* ASSOCIATE DATA WITH SHADER VARIABLES */
    
// ... other setup ...

// 1st attribute buffer : vertex positions
    glEnableVertexAttribArray(0); // Vertex positions at location 0
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Set up normals attribute at location 3
    glEnableVertexAttribArray(3); // Normals at location 3
    glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // Set up instance positions attribute at location 1
    glEnableVertexAttribArray(1); // Instance positions at location 1
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(1, 1);

    // Set up Chromosome IDs attribute at location 2
    glEnableVertexAttribArray(2); // Chromosome IDs at location 2
    glBindBuffer(GL_ARRAY_BUFFER, chromoIDVBO);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glVertexAttribDivisor(2, 1);

    // ... other setup ...

    /* UNBIND VERTEX-ARRAY OBJECT */
    
    glBindVertexArray(0);
    
    
    // Fetch uniform locations outside of the rendering loop


    GLuint scaleID = glGetUniformLocation(programID, "scale");
    GLuint VPMatrixID = glGetUniformLocation(programID, "VP");


    // Example light and view positions
    glm::vec3 lightPos = glm::vec3(10.0f, 10.0f, 10.0f); // Set to your light's position
    glm::vec3 viewPos = glm::vec3(0.0f, 0.0f, 5.0f); // Set to your camera's position

    //Draw Loop
    for (int i = 0; i < 5; ++i) { // Print first 5 positions for debugging
        std::cout << "Position " << i << ": " << instancePositions[i].x << ", " << instancePositions[i].y << ", " << instancePositions[i].z << std::endl;
    }
    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID);

        // Set the scale uniform (if used in your vertex shader)
        float scaleValue = 0.1f; // Adjust the scale as needed
        glUniform1f(scaleID, scaleValue);

        // Update light and view position uniforms
        glUniform3fv(lightPosID, 1, &lightPos[0]);
        glUniform3fv(viewPosID, 1, &viewPos[0]);

        // Update the camera's position based on input
        // (This could be done outside the loop, in response to input events)
        // cameraPos += ...;

        // Update the View matrix based on the camera's new position and orientation
        glm::mat4 View = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Update the Projection matrix based on the current zoom level (fov)
        glm::mat4 Projection = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);

        // Calculate View-Projection Matrix
        glm::mat4 VP = Projection * View;

        // Set VP matrix uniform
        glUniformMatrix4fv(VPMatrixID, 1, GL_FALSE, &VP[0][0]);

        // Bind VAO
        glBindVertexArray(VertexArrayID);

        // Draw the objects
        glDrawArraysInstanced(GL_TRIANGLES, 0, numVertices, instancePositions.size());

        // Unbind VAO
        glBindVertexArray(0);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
        glfwWindowShouldClose(window) == 0);


	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &normalsbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

