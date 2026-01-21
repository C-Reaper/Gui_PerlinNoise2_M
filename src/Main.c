#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/Random.h"
#include "/home/codeleaded/System/Static/Library/PerlinNoise.h"

#include "/home/codeleaded/System/Static/Library/Lib3D_Cube.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Mathlib.h"
#include "/home/codeleaded/System/Static/Library/Lib3D_Mesh.h"


Camera cam;
World3D world;	

int Mode = 0;
int Menu = 0;
float Speed = 4.0f;

Vec3D WorldOrigin;
Vec3D FunctionAngle;
Vec2 FunctionOrigin;

int Octaves = 7;
float Persistance = 50.0f;


float Function_2D(float x,float y){
	return PerlinNoise_2D_Get(x,y) * 2.0f;
}
void Menu_Set(int m){
	if(Menu==0 && m==1){
		AlxWindow_Mouse_SetInvisible(&window);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	if(Menu==1 && m==0){
		AlxWindow_Mouse_SetVisible(&window);
	}
	
	Menu = m;
}

void Setup(AlxWindow* w){
	Menu_Set(1);

    PerlinNoise_Permutations_Init();
	RGA_Set(Time_Nano());

	FunctionOrigin = (Vec2){ 0.0f,0.0f };
	FunctionAngle = (Vec3D){ 0.0f,0.0f,0.0f,1.0f };
	//WorldOrigin = (Vec3D){ 0.0f,0.0f,0.0f,1.0f };
	WorldOrigin = (Vec3D){ 0.0f,0.0f,0.0f,1.0f };

	cam = Camera_Make(
		(Vec3D){ 0.0f,15.0f,-18.0f,1.0f },
		(Vec3D){ 3.14 * 0.25f,0.0f,0.0f,1.0f },
		90.0f
	);

	world = World3D_Make(
		Matrix_MakeWorld(WorldOrigin,FunctionAngle),
		Matrix_MakePerspektive(cam.p,cam.up,cam.a),
		Matrix_MakeProjection(cam.fov,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f)
	);
	world.normal = WORLD3D_NORMAL_CAP;
}

void Update(AlxWindow* w){
	if(Menu==1){
		if(GetMouse().x!=GetMouseBefore().x || GetMouse().y!=GetMouseBefore().y){
			Vec2 d = Vec2_Sub(GetMouse(),GetMouseBefore());
			Vec2 a = Vec2_Mulf(Vec2_Div(d,(Vec2){ window.Width,window.Height }),12 * F64_PI);
	
			FunctionAngle.y += a.x;
			//FunctionAngle.x += a.y;
		}

		//Camera_Focus(&cam,GetMouseBefore(),GetMouse(),GetScreenRect().d);
		Camera_Update(&cam);
		SetMouse((Vec2){ GetWidth() / 2,GetHeight() / 2 });
	}
	
	if(Stroke(ALX_KEY_ESC).PRESSED)
		Menu_Set(!Menu);

	if(Stroke(ALX_KEY_Z).PRESSED)
		Mode = Mode < 3 ? Mode+1 : 0;

	if(Stroke(ALX_KEY_W).DOWN)
		FunctionOrigin.y += 7.0f * w->ElapsedTime;
	if(Stroke(ALX_KEY_S).DOWN)
		FunctionOrigin.y -= 7.0f * w->ElapsedTime;
	if(Stroke(ALX_KEY_A).DOWN)
		FunctionOrigin.x += 7.0f * w->ElapsedTime;
	if(Stroke(ALX_KEY_D).DOWN)
		FunctionOrigin.x -= 7.0f * w->ElapsedTime;

	if(Stroke(ALX_KEY_UP).PRESSED){
        Perlin_Offset++;
    }
    if(Stroke(ALX_KEY_DOWN).PRESSED){
        Perlin_Offset--;
    }
    if(Stroke(ALX_KEY_LEFT).PRESSED){
        Perlin_Persistance *= 1.01f;
    }
    if(Stroke(ALX_KEY_RIGHT).PRESSED){
        Perlin_Persistance *= 0.99f;
    }


	World3D_Set_Model(&world,Matrix_MakeWorld(WorldOrigin,FunctionAngle));
	World3D_Set_View(&world,Matrix_MakePerspektive(cam.p,cam.up,cam.a));
	World3D_Set_Proj(&world,Matrix_MakeProjection(cam.fov,(float)GetHeight() / (float)GetWidth(),0.1f,1000.0f));

	Vector_Clear(&world.trisIn);
	for(float i = -10.0f;i<=10.0f;i+=0.2f){
		for(float j = -10.0f;j<=10.0f;j+=0.2f){
			const float x1 = j;
			const float x2 = j+1;
			const float y1 = i;
			const float y2 = i+1;

			const Vec3D p1 = { x1,Function_2D(FunctionOrigin.x + x1,FunctionOrigin.y + y1),y1,1.0f };
			const Vec3D p2 = { x2,Function_2D(FunctionOrigin.x + x2,FunctionOrigin.y + y1),y1,1.0f };
			const Vec3D p3 = { x1,Function_2D(FunctionOrigin.x + x1,FunctionOrigin.y + y2),y2,1.0f };
			const Vec3D p4 = { x2,Function_2D(FunctionOrigin.x + x2,FunctionOrigin.y + y2),y2,1.0f };

			Tri3D t1 = { p1,p4,p2,{},RED };
			Tri3D t2 = { p1,p3,p4,{},RED };
			//Tri3D t3 = { p1,p2,p4,{},RED };
			//Tri3D t4 = { p1,p4,p3,{},RED };

			Tri3D_CalcNorm(&t1);
			Tri3D_CalcNorm(&t2);
			//Tri3D_CalcNorm(&t3);
			//Tri3D_CalcNorm(&t4);

			Tri3D_ShadeNorm(&t1,(Vec3D){ 0.5f,0.4f,0.6f,1.0f });
			Tri3D_ShadeNorm(&t2,(Vec3D){ 0.5f,0.4f,0.6f,1.0f });
			//Tri3D_ShadeNorm(&t3,(Vec3D){ 0.5f,0.4f,0.6f,1.0f });
			//Tri3D_ShadeNorm(&t4,(Vec3D){ 0.5f,0.4f,0.6f,1.0f });

			Vector_Push(&world.trisIn,&t1);
			Vector_Push(&world.trisIn,&t2);
			//Vector_Push(&world.trisIn,&t3);
			//Vector_Push(&world.trisIn,&t4);
		}
	}

	Clear(LIGHT_BLUE);

	World3D_update(&world,cam.p,(Vec2){ GetWidth(),GetHeight() });

	for(int i = 0;i<world.trisOut.size;i++){
		Tri3D* t = (Tri3D*)Vector_Get(&world.trisOut,i);
		const Pixel c = Pixel_Mulf(t->c.c,t->c.l);

		if(Mode==0)
			RenderTriangle(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c);
		if(Mode==1)
			RenderTriangleWire(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c,1.0f);
		if(Mode==2){
			RenderTriangle(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),c);
			RenderTriangleWire(((Vec2){ t->p[0].x, t->p[0].y }),((Vec2){ t->p[1].x, t->p[1].y }),((Vec2){ t->p[2].x, t->p[2].y }),WHITE,1.0f);
		}
	}

	// String str = String_Format("X: %f, Y: %f, Z: %f",cam.p.x,cam.p.y,cam.p.z);
	// RenderCStrSize(str.Memory,str.size,0,0,RED);
	// String_Free(&str);
	// str = String_Format("SizeIn: %d, SizeBuff: %d, SizeOut: %d",world.trisIn.size,world.trisBuff.size,world.trisOut.size);
	// RenderCStrSize(str.Memory,str.size,0,window.AlxFont.CharSizeY + 1,RED);
	// String_Free(&str);
	
    String str = String_Format("O:%d  P:%f",Octaves,Persistance);
    RenderCStrSize(str.Memory,str.size,0.0f,0.0f,RED);
    String_Free(&str);
}

void Delete(AlxWindow* w){
	World3D_Free(&world);
	AlxWindow_Mouse_SetVisible(&window);
}

int main(){
    if(Create("3D Meshing",2500,1200,1,1,Setup,Update,Delete))
        Start();
    return 0;
}