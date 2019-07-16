#include "stdafx.h"
#include "classes/system/Shader.h"
#include "classes/system/Scene.h"
#include "classes/system/FPSController.h"
#include "classes/buffers/StaticBuffer.h"

bool Pause;
bool keys[1024] = {0};
int WindowWidth = 800, WindowHeight = 600;
bool EnableVsync = 1;
GLFWwindow* window;
stFPSController FPSController;
MShader Shader;
MScene Scene;

//other variables
MStaticBuffer StaticBuffer;
glm::vec2 TileSize(32.0f, 32.0f);
vector<NRectangle2> Rooms;
vector<NRectangle2> Halls;

struct stHall {
	vector<NRectangle2> Parts;
	vector<NVector2> Connects;
};

stHall ConnectRooms2(NRectangle2* pRoom1, NRectangle2* pRoom2) {
	stHall Hall;
	if(!pRoom1 || !pRoom2) return Hall;
	if(pRoom1 == pRoom2) return Hall;

	vector<int> DataX;
	vector<int> DataY;
	vector<int> CopyDataX;
	vector<int> CopyDataY;
	int MaxHeight, MaxWidth;
	bool Vertical = false, Horizontal = false;
	NRectangle2 Hall0;
	NVector2 Connect0;
	
	DataX.push_back(pRoom1->Position.x);
	DataX.push_back(pRoom1->Position.x + pRoom1->Size.x);
	DataX.push_back(pRoom2->Position.x);
	DataX.push_back(pRoom2->Position.x + pRoom2->Size.x);
	
	DataY.push_back(pRoom1->Position.y);
	DataY.push_back(pRoom1->Position.y + pRoom1->Size.y);
	DataY.push_back(pRoom2->Position.y);
	DataY.push_back(pRoom2->Position.y + pRoom2->Size.y);
	
	CopyDataX.insert(CopyDataX.begin(), DataX.begin(), DataX.end());
	CopyDataY.insert(CopyDataY.begin(), DataY.begin(), DataY.end());
	
	sort(DataX.begin(), DataX.end());
	sort(DataY.begin(), DataY.end());
	
	if((CopyDataY[0] == DataY[2] && CopyDataY[1] == DataY[3]) || (CopyDataY[0] == DataY[0] && CopyDataY[1] == DataY[1])) {
		Vertical = true;
		MaxWidth = DataX[2] - DataX[1];
	}
	if((CopyDataX[0] == DataX[2] && CopyDataX[1] == DataX[3]) || (CopyDataX[0] == DataX[0] && CopyDataX[1] == DataX[1])) {
		Horizontal = true;
		MaxHeight = DataY[2] - DataY[1];
	}
	
	if(Vertical && !Horizontal) {
		cout<<"Vertical"<<endl;
		Hall0.Position.y = DataY[1];
		Hall0.Size.y = DataY[2] - DataY[1];
		Hall0.Size.x = (MaxWidth == 1) ? 1 : rand() % MaxWidth + 1;
		Hall0.Position.x = DataX[1];
		Hall.Parts.push_back(Hall0);
		Connect0 = NVector2(Hall0.Position.x, Hall0.Position.y - 1);
		Hall.Connects.push_back(Connect0);
		Connect0 = NVector2(Hall0.Position.x, Hall0.Position.y + Hall0.Size.y + 1);
		Hall.Connects.push_back(Connect0);
	}
	if(!Vertical && Horizontal) {
		cout<<"Horizontal"<<endl;
		Hall0.Position.x = DataX[1];
		Hall0.Size.x = DataX[2] - DataX[1];
		Hall0.Size.y = (MaxHeight == 1) ? 1 : rand() % MaxHeight + 1;
		Hall0.Position.y = DataY[1];
		Hall.Parts.push_back(Hall0);
		Connect0 = NVector2(Hall0.Position.x - 1, Hall0.Position.y);
		Hall.Connects.push_back(Connect0);
		Connect0 = NVector2(Hall0.Position.x + Hall0.Size.x + 1, Hall0.Position.y);
		Hall.Connects.push_back(Connect0);
	}
	if(Vertical && Horizontal) {
		cout<<"Complex"<<endl;
		NVector2 Center1(pRoom1->Position.x + pRoom1->Size.x * 0.5, pRoom1->Position.y + pRoom1->Size.y * 0.5);
		NVector2 Center2(pRoom2->Position.x + pRoom2->Size.x * 0.5, pRoom2->Position.y + pRoom2->Size.y * 0.5);
		NRectangle2 Hall1, Hall2;
		NVector2 Connect1, Connect2, Connect3;
		int rnd = rand() % 2;
		switch(rnd) {
			case 0://Center1.x, Center2.y
				Connect3 = NVector2(Center1.x, Center2.y);
				//room1 hall vertical
				if(Center2.y < Center1.y) {
					//room1 over room2
					Hall1.Size.y = pRoom1->Position.y - Center2.y;
					Hall1.Size.x = 1;
					Hall1.Position.x = Center1.x;
					Hall1.Position.y = Center2.y;
					Connect1 = NVector2(Hall1.Position.x, pRoom1->Position.y);
					cout<<"1y"<<endl;
				}
				else {
					//room2 over room1
					Hall1.Size.y = Center2.y - (pRoom1->Position.y + pRoom1->Size.y) + 1;
					Hall1.Size.x = 1;
					Hall1.Position.x = Center1.x;
					Hall1.Position.y = pRoom1->Position.y + pRoom1->Size.y;
					Connect1 = NVector2(Hall1.Position.x, pRoom1->Position.y + pRoom1->Size.y - 1);
					cout<<"2y"<<endl;
				}
				//room2 hall horizontal
				if(Center2.x < Center1.x) {
					//room1 right of room2
					Hall2.Size.x = Center1.x - (pRoom2->Position.x + pRoom2->Size.x);
					Hall2.Size.y = 1;
					Hall2.Position.x = pRoom2->Position.x + pRoom2->Size.x;
					Hall2.Position.y = Center2.y;
					Connect2 = NVector2(pRoom2->Position.x + pRoom2->Size.x - 1, Hall2.Position.y);
					cout<<"1x"<<endl;
				}
				else {
					//room2 right of room1
					Hall2.Size.x = pRoom2->Position.x - Center1.x - 1;
					Hall2.Size.y = 1;
					Hall2.Position.x = Center1.x + 1;
					Hall2.Position.y = Center2.y;
					Connect2 = NVector2(pRoom1->Position.x + pRoom1->Size.x + 1, Hall2.Position.y);
					cout<<"2x"<<endl;
				}
				break;
			case 1://Center2.x, Center1.y
				Connect3 = NVector2(Center2.x, Center1.y);
				//room2 hall vertical
				if(Center2.y < Center1.y) {
					//room1 over room2
					Hall2.Size.y = Center1.y - (pRoom2->Position.y + pRoom2->Size.y) + 1;
					Hall2.Size.x = 1;
					Hall2.Position.x = Center2.x;
					Hall2.Position.y = pRoom2->Position.y + pRoom2->Size.y;
					Connect2 = NVector2(Hall2.Position.x, pRoom2->Position.y + pRoom2->Size.y - 1);
					cout<<"3y"<<endl;
				}
				else {
					//room2 over room1
					Hall2.Size.y = pRoom2->Position.y - Center1.y;
					Hall2.Size.x = 1;
					Hall2.Position.x = Center2.x;
					Hall2.Position.y = Center1.y;
					Connect2 = NVector2(Hall2.Position.x, pRoom2->Position.y);
					cout<<"4y"<<endl;
				}
				//room1 hall horizontal
				if(Center2.x < Center1.x) {
					//room1 right of room2
					Hall1.Size.x = pRoom1->Position.x - Center2.x - 1;
					Hall1.Size.y = 1;
					Hall1.Position.x = Center2.x + 1;
					Hall1.Position.y = Center1.y;
					Connect1 = NVector2(pRoom1->Position.x, Hall1.Position.y);
					cout<<"3x"<<endl;
				}
				else {
					//room2 right of room1
					Hall1.Size.x = Center2.x - (pRoom1->Position.x + pRoom1->Size.x);
					Hall1.Size.y = 1;
					Hall1.Position.x = pRoom1->Position.x + pRoom1->Size.x;
					Hall1.Position.y = Center1.y;
					Connect1 = NVector2(pRoom1->Position.x + pRoom1->Size.x - 1, Hall1.Position.y);
					cout<<"4x"<<endl;
				}
				break;
		}
		Hall.Parts.push_back(Hall1);
		Hall.Parts.push_back(Hall2);
		Hall.Connects.push_back(Connect1);
		Hall.Connects.push_back(Connect3);//2
		Hall.Connects.push_back(Connect2);//3
	}
	
	return Hall;
}

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void mousepos_callback(GLFWwindow* window, double x, double y) {
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
		return;
	}
}

bool InitApp() {
	LogFile<<"Starting application"<<endl;    
    glfwSetErrorCallback(error_callback);
    
    if(!glfwInit()) return false;
    window = glfwCreateWindow(WindowWidth, WindowHeight, "TestApp", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return false;
    }
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mousepos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwMakeContextCurrent(window);
    if(glfwExtensionSupported("WGL_EXT_swap_control")) {
    	LogFile<<"Window: V-Sync supported. V-Sync: "<<EnableVsync<<endl;
		glfwSwapInterval(EnableVsync);//0 - disable, 1 - enable
	}
	else LogFile<<"Window: V-Sync not supported"<<endl;
    LogFile<<"Window created: width: "<<WindowWidth<<" height: "<<WindowHeight<<endl;

	//glew
	GLenum Error = glewInit();
	if(GLEW_OK != Error) {
		LogFile<<"Window: GLEW Loader error: "<<glewGetErrorString(Error)<<endl;
		return false;
	}
	LogFile<<"GLEW initialized"<<endl;
	
	if(!CheckOpenglSupport()) return false;
	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//shaders
	if(!Shader.CreateShaderProgram("shaders/main.vertexshader.glsl", "shaders/main.fragmentshader.glsl")) return false;
	if(!Shader.AddUnifrom("MVP", "MVP")) return false;
	LogFile<<"Shaders loaded"<<endl;

	//scene
	if(!Scene.Initialize(&WindowWidth, &WindowHeight)) return false;
	LogFile<<"Scene initialized"<<endl;

	//randomize
    srand(time(NULL));
    LogFile<<"Randomized"<<endl;
    
    //other initializations
    Rooms.push_back(NRectangle2(NVector2(0, 0), NVector2(3, 3)));
    Rooms.push_back(NRectangle2(NVector2(0, 9), NVector2(3, 3)));
    Rooms.push_back(NRectangle2(NVector2(4, 4), NVector2(4, 4)));
    Rooms.push_back(NRectangle2(NVector2(9, 9), NVector2(3, 3)));
    Rooms.push_back(NRectangle2(NVector2(9, 0), NVector2(3, 3)));
    vector<stHall> Halls;
    Halls.push_back(ConnectRooms2(&Rooms[0], &Rooms[2]));
    Halls.push_back(ConnectRooms2(&Rooms[1], &Rooms[2]));
    Halls.push_back(ConnectRooms2(&Rooms[3], &Rooms[2]));
    Halls.push_back(ConnectRooms2(&Rooms[4], &Rooms[2]));
    
    if(!StaticBuffer.Initialize()) return false;
	StaticBuffer.SetPrimitiveType(GL_QUADS);
	glm::vec3 Color(1, 1, 1);
	for(int i=0; i<Rooms.size(); i++) {
		StaticBuffer.AddVertex(glm::vec2(Rooms[i].Position.x * TileSize.x, Rooms[i].Position.y * TileSize.y), Color);
		StaticBuffer.AddVertex(glm::vec2(Rooms[i].Position.x * TileSize.x + Rooms[i].Size.x * TileSize.x, Rooms[i].Position.y * TileSize.y), Color);
		StaticBuffer.AddVertex(glm::vec2(Rooms[i].Position.x * TileSize.x + Rooms[i].Size.x * TileSize.x, Rooms[i].Position.y * TileSize.y + Rooms[i].Size.y * TileSize.y), Color);
		StaticBuffer.AddVertex(glm::vec2(Rooms[i].Position.x * TileSize.x, Rooms[i].Position.y * TileSize.y + Rooms[i].Size.y * TileSize.y), Color);
	}
	for(int i=0; i<Halls.size(); i++) {
		Color = glm::vec3(1, 0, 0);
		for(int j=0; j<Halls[i].Parts.size(); j++) {
			StaticBuffer.AddVertex(glm::vec2(Halls[i].Parts[j].Position.x * TileSize.x, Halls[i].Parts[j].Position.y * TileSize.y), Color);
			StaticBuffer.AddVertex(glm::vec2(Halls[i].Parts[j].Position.x * TileSize.x + Halls[i].Parts[j].Size.x * TileSize.x, Halls[i].Parts[j].Position.y * TileSize.y), Color);
			StaticBuffer.AddVertex(glm::vec2(Halls[i].Parts[j].Position.x * TileSize.x + Halls[i].Parts[j].Size.x * TileSize.x, Halls[i].Parts[j].Position.y * TileSize.y + Halls[i].Parts[j].Size.y * TileSize.y), Color);
			StaticBuffer.AddVertex(glm::vec2(Halls[i].Parts[j].Position.x * TileSize.x, Halls[i].Parts[j].Position.y * TileSize.y + Halls[i].Parts[j].Size.y * TileSize.y), Color);
		}
		Color = glm::vec3(0, 1, 0);
		for(int j=0; j<Halls[i].Connects.size(); j++) {
			StaticBuffer.AddVertex(glm::vec2(Halls[i].Connects[j].x * TileSize.x, Halls[i].Connects[j].y * TileSize.y), Color);
			StaticBuffer.AddVertex(glm::vec2((Halls[i].Connects[j].x + 1) * TileSize.x, Halls[i].Connects[j].y * TileSize.y), Color);
			StaticBuffer.AddVertex(glm::vec2((Halls[i].Connects[j].x + 1) * TileSize.x, (Halls[i].Connects[j].y + 1) * TileSize.y), Color);
			StaticBuffer.AddVertex(glm::vec2(Halls[i].Connects[j].x * TileSize.x, (Halls[i].Connects[j].y + 1) * TileSize.y), Color);
		}
	}
	//remove duplicate connect points
	/*
	vector<NVector2> AllConnects;
	for(int i=0; i<Halls.size(); i++) {
		AllConnects.insert(AllConnects.end(), Halls[i].Connects.begin(), Halls[i].Connects.end());
	}
	cout<<"before remove duplicate: "<<AllConnects.size()<<endl;
	sort(AllConnects.begin(), AllConnects.end(), NVector2Compare);
	AllConnects.erase(unique(AllConnects.begin(), AllConnects.end(), NVector2Equal), AllConnects.end());
	cout<<"after remove duplicates: "<<AllConnects.size()<<endl;
	for(int i=0; i<AllConnects.size(); i++) {
		cout<<AllConnects[i].x<<" "<<AllConnects[i].y<<endl;
	}
	*/
	//fill map
	//map<NVector2*, vector<NVector2*> > ConnectsMap;
	
	if(!StaticBuffer.Dispose()) return false;
	
	//turn off pause
	Pause = false;
    
    return true;
}

void RenderStep() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(Shader.ProgramId);
	glUniformMatrix4fv(Shader.Uniforms["MVP"], 1, GL_FALSE, Scene.GetDynamicMVP());
	
	//draw functions
	StaticBuffer.Begin();
	StaticBuffer.Draw();
	StaticBuffer.End();
}

void ClearApp() {
	//clear funstions
	StaticBuffer.Close();
	Rooms.clear();
	Halls.clear();
	
	memset(keys, 0, 1024);
	Shader.Close();
	LogFile<<"Application: closed"<<endl;
}

int main(int argc, char** argv) {
	LogFile<<"Application: started"<<endl;
	if(!InitApp()) {
		ClearApp();
		glfwTerminate();
		LogFile.close();
		return 0;
	}
	FPSController.Initialize(glfwGetTime());
	while(!glfwWindowShouldClose(window)) {
		FPSController.FrameStep(glfwGetTime());
    	FPSController.FrameCheck();
		RenderStep();
        glfwSwapBuffers(window);
        glfwPollEvents();
	}
	ClearApp();
    glfwTerminate();
    LogFile.close();
    return 0;
}
