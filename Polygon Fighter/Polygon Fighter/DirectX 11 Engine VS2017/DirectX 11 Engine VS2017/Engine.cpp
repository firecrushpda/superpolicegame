//=============================================================================
//
// エンジン処理 [Engine.cpp]
// Author : GP11B183 28 潘 唯多 (パン　イタ)
//
//=============================================================================
#include "Engine.h"

//=============================================================================
// エンジン初期化処理
//=============================================================================
bool Engine::Initialize(HINSTANCE hInstance, std::string window_title, std::string window_class, int width, int height)
{
	
	timer.Start();
	chasecarcdtimer.Start();

	if (!this->render_window.Initialize(this, hInstance, window_title, window_class, width, height))
		return false;

	if (!gfx.Initialize(this->render_window.GetHWND(), width, height))
		return false;

	gfx.ResetTitle();

	//change input setting
	LoadKeyboardLayout(_T("0x0409"), KLF_ACTIVATE);

	return true;
}

//=============================================================================
// メッセージ処理
//=============================================================================
bool Engine::ProcessMessages()
{
	return this->render_window.ProcessMessages();
}

//=============================================================================
// 更新処理
//=============================================================================
void Engine::Update()
{
	// 時間の初期化
	float dt = 1.0;//timer.GetMilisecondsElapsed();

	gfx.m_Sound->Update();

	gfx.pgotest.Update();

	gfx.physxbase.stepPhysics();

	if (gfx.gs == GameState::title)
	{
		while (!keyboard.KeyBufferIsEmpty())
		{
			KeyboardEvent kbe = keyboard.ReadKey();
			unsigned char keycode = kbe.GetKeyCode();
			if (kbe.IsPress())
			{
				if (keycode == '\r')
				{
					gfx.Fade(GameState::game);
					//gfx.Fade(GameState::editor);
					////temp
					//gfx.m_editor.InitializeEditor(gfx.GetDevice(),gfx.GetDeviceContent(), gfx.cb_vs_vertexshader);
				}
				if (keycode == 'T')
				{
					gfx.Fade(GameState::tutorial);
				}
			}
		}
	}

	if (gfx.gs == GameState::tutorial)
	{
		while (!keyboard.KeyBufferIsEmpty())
		{
			KeyboardEvent kbe = keyboard.ReadKey();
			unsigned char keycode = kbe.GetKeyCode();
			if (kbe.IsPress())
			{
				if (keycode == '\r')
				{
					gfx.tutorialtexno++;
					if (gfx.tutorialtexno >= 2)
						gfx.Fade(GameState::title);
				}
			}
		}
	}

	if (gfx.gs == GameState::score)
	{
		if (gfx.scorepageindex == 0)
		{
			while (!keyboard.KeyBufferIsEmpty())
			{
				KeyboardEvent kbe = keyboard.ReadKey();
				unsigned char keycode = kbe.GetKeyCode();
				if (kbe.IsPress())
				{
					if (keycode == '\r')
					{
						gfx.scorepageindex++;
						/*if (gfx.scorepageindex > 1)
							gfx.Fade(GameState::title);*/
					}
				}
			}

			int score = gfx.gamescore + 100;
			score >= 999999 ? score = 999999 : score = score;
			for (size_t i = 0; i < 6; i++)
			{
				float dig = (float)(score % 10);
				score = (int)score / 10;
				std::vector<Vertex2D> vertexData =
				{
					Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
					Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
					Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
					Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
				};
				gfx.scoredigf[i].UpdateUV(vertexData);
			}

			float totalscore = 100 + gfx.gamescore * (gfx.car.catchcount);
			auto tscore = totalscore;
			tscore >= 999999 ? tscore = 999999 : tscore = tscore;
			for (size_t i = 0; i < 6; i++)
			{
				float dig = (float)((int)tscore % 10);
				tscore = (int)tscore / 10;
				std::vector<Vertex2D> vertexData =
				{
					Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
					Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
					Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
					Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
				};
				gfx.tscoredigf[i].UpdateUV(vertexData);
			}

			std::vector<Vertex2D> vertexData =
			{
				Vertex2D(-0.5f, -0.5f, 0.0f, gfx.car.catchcount * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
				Vertex2D(0.5f, -0.5f, 0.0f, gfx.car.catchcount * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
				Vertex2D(-0.5, 0.5, 0.0f, gfx.car.catchcount * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
				Vertex2D(0.5f, 0.5, 0.0f, gfx.car.catchcount * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
			};
			gfx.catchcardig.UpdateUV(vertexData);

			if (totalscore < 1000 && totalscore >= 0)
			{
				gfx.rank1idx = 3;
			}
			else if (totalscore < 2000 && totalscore >= 1000)
			{
				gfx.rank1idx = 2;
			}
			else if (totalscore < 3000 && totalscore >= 2000)
			{
				gfx.rank1idx = 1;
			}
			else if (totalscore < 4000 && totalscore >= 3000)
			{
				gfx.rank1idx = 0;
			}
			else if (totalscore >= 4000)
			{
				gfx.rank1idx = 18;
			}

			std::vector<Vertex2D> vertexData1 =
			{
				Vertex2D(-0.5f, -0.5f, 0.0f, gfx.rank1idx * 1.0f / 28.0f + 0.0f, 0.0f), //TopLeft
				Vertex2D(0.5f, -0.5f, 0.0f, gfx.rank1idx * 1.0f / 28.0f + 1.0f / 28.0f , 0.0f), //TopRight
				Vertex2D(-0.5, 0.5, 0.0f, gfx.rank1idx * 1.0f / 28.0f + 0.0f, 1.0f), //Bottom Left 
				Vertex2D(0.5f, 0.5, 0.0f, gfx.rank1idx * 1.0f / 28.0f + 1.0f / 28.0f , 1.0f), //Bottom Right
			};
			gfx.rank1.UpdateUV(vertexData1);
		}
		else if (gfx.scorepageindex == 1)
		{
		/*	if (gfx.editname)
			{
				
			}
			else
			{*/
				//top1
				for (size_t i = 0; i < 3; i++)
				{
					std::vector<Vertex2D> vertexData1 =
					{
						Vertex2D(-0.5f, -0.5f, 0.0f, gfx.top1[i] * 1.0f / 28.0f + 0.0f, 0.0f), //TopLeft
						Vertex2D(0.5f, -0.5f, 0.0f, gfx.top1[i] * 1.0f / 28.0f + 1.0f / 28.0f , 0.0f), //TopRight
						Vertex2D(-0.5, 0.5, 0.0f, gfx.top1[i] * 1.0f / 28.0f + 0.0f, 1.0f), //Bottom Left 
						Vertex2D(0.5f, 0.5, 0.0f, gfx.top1[i] * 1.0f / 28.0f + 1.0f / 28.0f , 1.0f), //Bottom Right
					};
					gfx.top1name[i].UpdateUV(vertexData1);
				}

				auto tscore = gfx.top1score;
				tscore >= 999999 ? tscore = 999999 : tscore = tscore;
				for (size_t i = 0; i < 6; i++)
				{
					float dig = (float)((int)tscore % 10);
					tscore = (int)tscore / 10;
					std::vector<Vertex2D> vertexData =
					{
						Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
						Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
						Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
						Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
					};
					gfx.top1scoredigf[i].UpdateUV(vertexData);
				}

				//top2
				for (size_t i = 0; i < 3; i++)
				{
					std::vector<Vertex2D> vertexData1 =
					{
						Vertex2D(-0.5f, -0.5f, 0.0f, gfx.top2[i] * 1.0f / 28.0f + 0.0f, 0.0f), //TopLeft
						Vertex2D(0.5f, -0.5f, 0.0f, gfx.top2[i] * 1.0f / 28.0f + 1.0f / 28.0f , 0.0f), //TopRight
						Vertex2D(-0.5, 0.5, 0.0f, gfx.top2[i] * 1.0f / 28.0f + 0.0f, 1.0f), //Bottom Left 
						Vertex2D(0.5f, 0.5, 0.0f, gfx.top2[i] * 1.0f / 28.0f + 1.0f / 28.0f , 1.0f), //Bottom Right
					};
					gfx.top2name[i].UpdateUV(vertexData1);
				}

				tscore = gfx.top2score;
				tscore >= 999999 ? tscore = 999999 : tscore = tscore;
				for (size_t i = 0; i < 6; i++)
				{
					float dig = (float)((int)tscore % 10);
					tscore = (int)tscore / 10;
					std::vector<Vertex2D> vertexData =
					{
						Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
						Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
						Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
						Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
					};
					gfx.top2scoredigf[i].UpdateUV(vertexData);
				}

				//top3
				for (size_t i = 0; i < 3; i++)
				{
					std::vector<Vertex2D> vertexData1 =
					{
						Vertex2D(-0.5f, -0.5f, 0.0f, gfx.top3[i] * 1.0f / 28.0f + 0.0f, 0.0f), //TopLeft
						Vertex2D(0.5f, -0.5f, 0.0f, gfx.top3[i] * 1.0f / 28.0f + 1.0f / 28.0f , 0.0f), //TopRight
						Vertex2D(-0.5, 0.5, 0.0f, gfx.top3[i] * 1.0f / 28.0f + 0.0f, 1.0f), //Bottom Left 
						Vertex2D(0.5f, 0.5, 0.0f, gfx.top3[i] * 1.0f / 28.0f + 1.0f / 28.0f , 1.0f), //Bottom Right
					};
					gfx.top3name[i].UpdateUV(vertexData1);
				}

				tscore = gfx.top3score;
				tscore >= 999999 ? tscore = 999999 : tscore = tscore;
				for (size_t i = 0; i < 6; i++)
				{
					float dig = (float)((int)tscore % 10);
					tscore = (int)tscore / 10;
					std::vector<Vertex2D> vertexData =
					{
						Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
						Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
						Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
						Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
					};
					gfx.top3scoredigf[i].UpdateUV(vertexData);
				}

				//top4
				for (size_t i = 0; i < 3; i++)
				{
					std::vector<Vertex2D> vertexData1 =
					{
						Vertex2D(-0.5f, -0.5f, 0.0f, gfx.top4[i] * 1.0f / 28.0f + 0.0f, 0.0f), //TopLeft
						Vertex2D(0.5f, -0.5f, 0.0f, gfx.top4[i] * 1.0f / 28.0f + 1.0f / 28.0f , 0.0f), //TopRight
						Vertex2D(-0.5, 0.5, 0.0f, gfx.top4[i] * 1.0f / 28.0f + 0.0f, 1.0f), //Bottom Left 
						Vertex2D(0.5f, 0.5, 0.0f, gfx.top4[i] * 1.0f / 28.0f + 1.0f / 28.0f , 1.0f), //Bottom Right
					};
					gfx.top4name[i].UpdateUV(vertexData1);
				}

				tscore = gfx.top4score;
				tscore >= 999999 ? tscore = 999999 : tscore = tscore;
				for (size_t i = 0; i < 6; i++)
				{
					float dig = (float)((int)tscore % 10);
					tscore = (int)tscore / 10;
					std::vector<Vertex2D> vertexData =
					{
						Vertex2D(-0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 0.0f, 0.0f), //TopLeft
						Vertex2D(0.5f, -0.5f, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 0.0f), //TopRight
						Vertex2D(-0.5, 0.5, 0.0f, dig * 1.0f / 11.0f + 0.0f, 1.0f), //Bottom Left 
						Vertex2D(0.5f, 0.5, 0.0f, dig * 1.0f / 11.0f + 1.0f / 11.0f , 1.0f), //Bottom Right
					};
					gfx.top4scoredigf[i].UpdateUV(vertexData);
				}

				//quit
				while (!keyboard.KeyBufferIsEmpty())
				{
					KeyboardEvent kbe = keyboard.ReadKey();
					unsigned char keycode = kbe.GetKeyCode();
					if (kbe.IsPress())
					{
						if (keycode == '\r')
						{
							gfx.scorepageindex++;
							gfx.gamescore = 0;
							gfx.car.catchcount = 0;
							//gfx.savescore
							if (gfx.scorepageindex > 1)
								gfx.Fade(GameState::title);
						}
					}
				}
			//}

			
		}
	}
#pragma region GameState::game
	if (gfx.gs == GameState::game)
	{
		if (!gfx.gamepause)
		{
			auto cameratype = gfx.Camera3D.cameratype;

			//update dynamic physxobject
			PxShape* shapes[128];
			unsigned int index = 0;
			for (size_t i = 0; i < gfx.obstaclephysics.size(); i++)
			{
				auto phyobs = gfx.obstaclephysics.at(i);

				const PxU32 nbShapes = phyobs->getNbShapes();
				PX_ASSERT(nbShapes <= 128);
				phyobs->getShapes(shapes, nbShapes);
				const bool sleeping = phyobs->is<PxRigidDynamic>() ? phyobs->is<PxRigidDynamic>()->isSleeping() : false;

				for (PxU32 j = 0; j < nbShapes; j++)
				{
					const PxMat44 shapePose(PxShapeExt::getGlobalPose(*shapes[j], *phyobs));
					const PxGeometryHolder h = shapes[j]->getGeometry();

					//auto mat = GetMatrixFromPxMatrix(shapePose);
					auto mat = XMMatrixSet(shapePose.column0.x, shapePose.column0.y, shapePose.column0.z, shapePose.column0.w,
										   shapePose.column1.x, shapePose.column1.y, shapePose.column1.z, shapePose.column1.w,
										   shapePose.column2.x, shapePose.column2.y, shapePose.column2.z, shapePose.column2.w,
										   shapePose.column3.x, shapePose.column3.y, shapePose.column3.z, shapePose.column3.w);
					//gfx.obstaclego.at(index)->GetMesh().at(0).transformMatrix = mat;
					gfx.obstaclego.at(index)->SetWorldMatirx(mat);
					index++;
				}
			}
			

			// キーボードの読み込む
			while (!keyboard.CharBufferIsEmpty())
			{
				unsigned char ch = keyboard.ReadChar();
			}

			//key press
			while (!keyboard.KeyBufferIsEmpty())
			{
				KeyboardEvent kbe = keyboard.ReadKey();
				unsigned char keycode = kbe.GetKeyCode();
				if (kbe.IsPress())
				{
					if (keycode == 'L')
					{
						gfx.gamepause = !gfx.gamepause;
					}
					if (keycode == 'O')
					{
						gfx.showImgui = !gfx.showImgui;
					}
					if (keycode == 'P')
					{
						gfx.car.carrender.SetCollisionBoxView(true);
						for (size_t i = 0; i < gfx.mapgo.size(); i++)
						{
							gfx.mapgo.at(i)->SetCollisionBoxView(true);
						}
					}
					if (keycode == 'U')
					{
						gfx.car.StartDissolveAnimaion();
					}

					//camera
					if (keycode == '0')//follow
					{
						gfx.Camera3D.cameratype = 0;
						gfx.car.carbar_drawflag = false;
					}
					if (keycode == '1')//free
					{
						gfx.Camera3D.cameratype = 1;
						gfx.car.carbar_drawflag = false;
					}
					if (keycode == '2')//front
					{
						gfx.Camera3D.cameratype = 2;
						gfx.car.carbar_drawflag = true;
					}
					if (keycode == '4')//camerawork point
					{
						gfx.Camera3D.cameratype = 4;
						gfx.car.carbar_drawflag = false;
						gfx.Camera3D.timer.Restart();
						for (size_t i = 0; i < gfx.mapgo.size(); i++)
						{
							if (StringHelper::IsContain(gfx.mapgo.at(i)->path, gfx.Camera3D.focusname))
							{
								gfx.Camera3D.focusgo = gfx.mapgo.at(i);
								break;
							}
						}
					}
					if (keycode == '5')//camerawork line
					{
						gfx.Camera3D.cameratype = 5;
						//gfx.Camera3D.focusgo = &gfx.car.carrender;
						gfx.car.carbar_drawflag = false;
						gfx.Camera3D.timer.Restart();
						for (size_t i = 0; i < gfx.mapgo.size(); i++)
						{
							if (StringHelper::IsContain(gfx.mapgo.at(i)->path, gfx.Camera3D.focusname))
							{
								gfx.Camera3D.focusgo = gfx.mapgo.at(i);
								break;
							}
						}
						gfx.cac->canmove = !gfx.cac->canmove;
					}
					if (keycode == '6')//camerawork rotate
					{
						gfx.Camera3D.cameratype = 6;
						//gfx.Camera3D.focusgo = &gfx.car.carrender;
						gfx.car.carbar_drawflag = false;
						gfx.Camera3D.timer.Restart();
						for (size_t i = 0; i < gfx.mapgo.size(); i++)
						{
							if (StringHelper::IsContain(gfx.mapgo.at(i)->path, gfx.Camera3D.focusname))
							{
								gfx.Camera3D.focusgo = gfx.mapgo.at(i);
								break;
							}
						}
					}


					if (keycode == '\r')
					{
						gfx.Fade(GameState::score);
					}
				}

				if (kbe.IsRelease())
				{

				}

			}

			//マウスの読み込む
			while (!mouse.EventBufferIsEmpty())
			{
				MouseEvent me = mouse.ReadEvent();

				if (mouse.IsRightDown())
				{
					if (cameratype == 1)
					{
						if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
						{
							this->gfx.Camera3D.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0);
						}
					}
				}
			}

			float Camera3DSpeed = 0.06f;

			//press
			if (cameratype == 1)
			{
				if (keyboard.KeyIsPressed('W'))
				{
					this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetForwardVector() * Camera3DSpeed * dt);
				}
				if (keyboard.KeyIsPressed('S'))
				{
					this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
				}
				if (keyboard.KeyIsPressed('A'))
				{
					this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetLeftVector() * Camera3DSpeed * dt);
				}
				if (keyboard.KeyIsPressed('D'))
				{
					this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetRightVector() * Camera3DSpeed * dt);
				}
				if (keyboard.KeyIsPressed(VK_SPACE))
				{
					this->gfx.Camera3D.AdjustPosition(0.0f, Camera3DSpeed * dt, 0.0f);
				}
				if (keyboard.KeyIsPressed('Z'))
				{
					this->gfx.Camera3D.AdjustPosition(0.0f, -Camera3DSpeed * dt, 0.0f);
				}
			}

			//fix camera
			if (cameratype == 0 || cameratype == 4 || cameratype == 5 || cameratype == 6)
			{
				//control myself
				PxRigidDynamic* Actor = gfx.car.actor;
				physx::PxTransform transform = Actor->getGlobalPose();

				PxVec3 dir = transform.q.rotate(PxVec3(0.0f, 0.0f, 1.0f));
				PxVec3 vel = Actor->getLinearVelocity();

				gfx.physxbase.gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);

				
				if (keyboard.KeyIsPressed('W'))
				{
					gfx.physxbase.gVehicle4W->getRigidDynamicActor()->addForce(dir * 10000);
					if (keyboard.KeyIsPressed('A'))
					{
						gfx.physxbase.startTurnHardLeftMode();
					}
					else if (keyboard.KeyIsPressed('D'))
					{
						gfx.physxbase.startTurnHardRightMode();
					}
					else
					{
						gfx.physxbase.releaseAllControls();
						gfx.physxbase.startAccelerateForwardsMode();
					}
				}
				else if (keyboard.KeyIsPressed('S'))
				{
					gfx.physxbase.gVehicle4W->getRigidDynamicActor()->addForce(dir * -10000);
					gfx.physxbase.gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
					if (keyboard.KeyIsPressed('A'))
					{
						gfx.physxbase.startTurnHardLeftMode();
					}
					else if (keyboard.KeyIsPressed('D'))
					{
						gfx.physxbase.startTurnHardRightMode();
					}
					else
					{
						gfx.physxbase.releaseAllControls();
						gfx.physxbase.startAccelerateReverseMode();
					}
					
				}
				else
				{
					gfx.physxbase.releaseAllControls();
				}
			}

			if (cameratype == 2)
			{

				gfx.physxbase.gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eFIRST);
				if (keyboard.KeyIsPressed('W'))
				{
					if (keyboard.KeyIsPressed('A'))
					{
						gfx.physxbase.startTurnHardLeftMode();
					}
					else if (keyboard.KeyIsPressed('D'))
					{
						gfx.physxbase.startTurnHardRightMode();
					}
					else
					{
						gfx.physxbase.startAccelerateForwardsMode();
					}
				}
				else if (keyboard.KeyIsPressed('S'))
				{

					if (keyboard.KeyIsPressed('A'))
					{
						gfx.physxbase.startTurnHardLeftMode();
					}
					else if (keyboard.KeyIsPressed('D'))
					{
						gfx.physxbase.startTurnHardRightMode();
					}
					else
					{
						gfx.physxbase.startAccelerateReverseMode();
					}
					gfx.physxbase.gVehicle4W->mDriveDynData.forceGearChange(PxVehicleGearsData::eREVERSE);
				}
				else
				{
					gfx.physxbase.releaseAllControls();
				}
				/*if (keyboard.KeyIsPressed('W'))
				{
					this->gfx.car.MoveFowards(dt, 1.0f, gfx.mapgo);
					mIsInput = true;
				}
				else if (keyboard.KeyIsPressed('S'))
				{
					this->gfx.car.MoveFowards(dt, -1.0f, gfx.mapgo);
					mIsInput = true;
				}

				if (keyboard.KeyIsPressed('A'))
				{
					this->gfx.car.Turn(dt, -1.0f);
				}
				else if (keyboard.KeyIsPressed('D'))
				{
					this->gfx.car.Turn(dt, 1.0f);
				}
				else
				{
					this->gfx.car.Turn(dt, 0.0f);
				}*/

				auto viewrot = gfx.Camera3D.roundviewrot;
				auto carrot = gfx.car.carrender.GetRotationFloat3();
				if (viewrot.y != carrot.y)
				{
					auto viewrotV = XMLoadFloat3(&viewrot);
					auto carrotV = XMLoadFloat3(&carrot);
					auto lerpvalue = XMVectorLerp(viewrotV, carrotV, 0.035f);
					XMStoreFloat3(&gfx.Camera3D.roundviewrot, lerpvalue);
				}

				auto dfront = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(gfx.Camera3D.roundviewrot.x, gfx.Camera3D.roundviewrot.y, 0.0f);
				auto vec_front = XMVector3TransformCoord(dfront, vecRotationMatrix);

				auto testpos = gfx.car.carrender.GetPositionVector() + vec_front * gfx.Camera3D.cf_front;
				DirectX::XMFLOAT3 temp;
				DirectX::XMStoreFloat3(&temp, testpos);
				temp = DirectX::XMFLOAT3(temp.x, temp.y + gfx.Camera3D.cf_fheight, temp.z);
				gfx.Camera3D.SetPosition(temp);

				//car bar pos
				auto barpos = gfx.car.carrender.GetPositionVector() + vec_front * (gfx.Camera3D.cf_front + 0.3);
				DirectX::XMStoreFloat3(&temp, barpos);
				temp = DirectX::XMFLOAT3(temp.x, temp.y - 0.2, temp.z);

				//rot
				auto lacarbarrotZ = gfx.car.carbar.GetRotationFloat3().z;
				lacarbarrotZ = lacarbarrotZ * (1 - 0.05);
				gfx.car.carbar.SetPosition(temp);
				gfx.car.carbar.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
				auto lacarbarrot = gfx.car.carbar.GetRotationFloat3();

				gfx.car.carbar.SetRotation(XMFLOAT3(lacarbarrot.x, lacarbarrot.y + XM_PI, lacarbarrotZ));//-gfx.Camera3D.roundviewrot.y

				testpos = gfx.car.carrender.GetPositionVector() + vec_front * gfx.Camera3D.cf_front * 10;
				XMStoreFloat3(&temp, testpos);
				gfx.Camera3D.SetLookAtPos(temp);
			}

			// camera works point
			if (cameratype == 4)
			{
				if (gfx.Camera3D.timer.GetMilisecondsElapsed() >= gfx.Camera3D.cwwaittime * 1000)
				{
					gfx.Camera3D.timer.Restart();
					if (gfx.Camera3D.cwpointindex >= gfx.Camera3D.mCameraWorkTrack_Point.size() - 1)
					{
						gfx.Camera3D.cwpointindex = 0;
						return;
					}
					else
					{
						gfx.Camera3D.cwpointindex++;
						return;
					}
				}
				gfx.Camera3D.SetPosition(gfx.Camera3D.mCameraWorkTrack_Point.at(gfx.Camera3D.cwpointindex));
				gfx.Camera3D.SetLookAtPos(gfx.Camera3D.focusgo->GetPositionFloat3());
			}	

			//camera works line
			if (cameratype == 5)
			{
				float epsilon = 1.0f;
				XMFLOAT3 dir;

				auto campos = gfx.Camera3D.GetPositionFloat3();
				auto camrot = gfx.Camera3D.GetRotationFloat3();
				auto tgpos = gfx.Camera3D.mCameraWorkTrack_Line.at(gfx.Camera3D.cwlineindex);

				dir.x = tgpos.x - campos.x;
				dir.y = tgpos.y - campos.y;
				dir.z = tgpos.z - campos.z;

				float len = sqrt(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
				dir.x /= len;
				dir.y /= len;
				dir.z /= len;

				campos.x += dir.x * gfx.Camera3D.cwlineSpeed * 1.0f / 60.0f;
				campos.y += dir.y * gfx.Camera3D.cwlineSpeed * 1.0f / 60.0f;
				campos.z += dir.z * gfx.Camera3D.cwlineSpeed * 1.0f / 60.0f;

				gfx.Camera3D.SetPosition(campos);
				if (abs(campos.x - tgpos.x) <= 1.0f && abs(campos.y - tgpos.y) <= 1.0f && abs(campos.z - tgpos.z) <= 1.0f)
				{
					if (gfx.Camera3D.cwlineindex >= gfx.Camera3D.mCameraWorkTrack_Line.size() - 1)
					{
						gfx.Camera3D.cwlineindex = 0;
						return;
					}
					else
					{
						gfx.Camera3D.cwlineindex++;
						return;
					}
				}
				//gfx.Camera3D.SetPosition(gfx.Camera3D.mCameraWorkTrack_Line.at(gfx.Camera3D.cwlineindex));
				gfx.Camera3D.SetLookAtPos(gfx.Camera3D.focusgo->GetPositionFloat3());
			}

			//camera works rotate
			if (cameratype == 6)
			{
				auto viewrot = gfx.Camera3D.roundviewrot;
				gfx.Camera3D.roundviewrot = XMFLOAT3(viewrot.x, viewrot.y + gfx.Camera3D.cwrotatespeed, viewrot.z);

				auto dback = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(gfx.Camera3D.roundviewrot.x, gfx.Camera3D.roundviewrot.y, 0.0f);
				auto vec_backward = XMVector3TransformCoord(dback, vecRotationMatrix);

				auto yjiku = XMVectorSet(0, gfx.Camera3D.cwrotateheight,0,0);
				auto testpos = gfx.Camera3D.focusgo->GetPositionVector() + vec_backward * gfx.Camera3D.cwrotatedistance + yjiku;
				DirectX::XMFLOAT3 temp;
				DirectX::XMStoreFloat3(&temp, testpos);
				gfx.Camera3D.SetPosition(temp);
				gfx.Camera3D.SetLookAtPos(gfx.Camera3D.focusgo->GetPositionFloat3());

			}

			//update car
			this->gfx.car.Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());

			//update car speed ui
			auto vel = gfx.car.GetCarVelocity();
			auto maxspeed = gfx.car.GetMaxSpeed();
			gfx.car.carsui.Update(1.0f - std::abs(vel) / maxspeed, std::abs(vel), 
								gfx.car.catchcount,gfx.cac->countdown);

			//update money ui
			gfx.moneyui.Update();

			//update chase car
			gfx.cac->mAICar->Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());
			gfx.cac->Update(1.0f / 60.0f);

			//update chase car position sign
			{
				auto chasecarvecpos = gfx.cac->mAICar->carrender.GetPositionVector();
				auto camviewport = gfx.Camera3D.viewport;
				auto vec = XMVector3Project(chasecarvecpos, camviewport.TopLeftX, camviewport.TopLeftY,
					camviewport.Width, camviewport.Height, camviewport.MinDepth, camviewport.MaxDepth,
					gfx.Camera3D.GetProjectionMatrix(), gfx.Camera3D.GetViewMatrix(), DirectX::XMMatrixIdentity());
				XMFLOAT3 co;
				XMStoreFloat3(&co, vec);
				co = XMFLOAT3(clamp(co.x - 75/2, -200.0f, gfx.windowWidth + 100.0f), clamp(co.y - 77/2, -200.0f, gfx.windowHeight + 100.0f) - 50, 0);
				gfx.cac->possign.SetPosition(co);
			}

			if (cameratype == 0)
			{
				//camrot
				auto viewrot = gfx.Camera3D.roundviewrot;
				auto carrot = gfx.car.carrender.GetRotationFloat3();
				if (viewrot.y != carrot.y)
				{
					auto viewrotV = XMLoadFloat3(&viewrot);
					auto carrotV = XMLoadFloat3(&carrot);
					auto lerpvalue = XMVectorLerp(viewrotV, carrotV, 0.035f);
					XMStoreFloat3(&gfx.Camera3D.roundviewrot, lerpvalue);
				}

				//campos
				auto dback = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(gfx.Camera3D.roundviewrot.x, gfx.Camera3D.roundviewrot.y, 0.0f);
				auto vec_backward = XMVector3TransformCoord(dback, vecRotationMatrix);
				XMVECTOR testpos;
				if (gfx.car.GetCarVelocity() <= 17.0f)
				{
					testpos = gfx.car.carrender.GetPositionVector() + vec_backward * gfx.Camera3D.cf_back;
				}
				else
				{
					testpos = gfx.car.carrender.GetPositionVector() + vec_backward * gfx.Camera3D.cf_back * gfx.car.GetCarVelocity() / 17.0f;
				}
				
				
				DirectX::XMFLOAT3 temp;
				DirectX::XMStoreFloat3(&temp, testpos);
				temp = DirectX::XMFLOAT3(temp.x, temp.y + gfx.Camera3D.cf_height, temp.z);
				gfx.Camera3D.SetPosition(temp);
				gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
			}

			if (cameratype == 3)
			{
				auto npc = gfx.npc.at(gfx.currentnpcindex);
				auto dback = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
				XMMATRIX vecRotationMatrix = XMMatrixRotationRollPitchYaw(npc->girl.GetRotationFloat3().x, npc->girl.GetRotationFloat3().y, 0.0f);
				auto vec_forward = XMVector3TransformCoord(dback, vecRotationMatrix);
				auto testpos = npc->girl.GetPositionVector() + vec_forward * 10 + DirectX::XMVectorSet(0.0f, 5.0f, 0.0f, 0.0f);//
				gfx.Camera3D.SetPosition(testpos);

				//camrot
				auto viewrot = gfx.Camera3D.roundviewrot;
				auto lookAtPos = npc->girl.GetPositionFloat3();
				auto pos = gfx.Camera3D.GetPositionFloat3();

				gfx.Camera3D.SetLookAtPos(lookAtPos);

			}

			//update map game object
			for (size_t i = 0; i < gfx.mapgo.size(); i++)
			{
				//gfx.mapgo.at(i)->AdjustRotation(XMFLOAT3(0, 0.01, 0));
				gfx.mapgo.at(i)->Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());
			}

			//update npc
			for (size_t i = 0; i < gfx.npc.size(); i++)
				gfx.npc.at(i)->Update(1.0f, gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix(),
									gfx.car.carrender.GetPositionFloat3());

			//collision
			//衝突判定
			auto cocar = gfx.car.carrender.GetCollisionObject();
			auto cochasecar = gfx.cac->mAICar->carrender.GetCollisionObject();
			auto cocamera = gfx.Camera3D.GetCameraCollision();

			//frustum culling
			for (size_t i = 0; i < gfx.mapgo.size(); i++)
			{
				auto obb = gfx.mapgo.at(i)->GetCollisionObject()->obb;
				DirectX::ContainmentType coresult = cocamera->frustum.Contains(obb);
				if (coresult == 2 || coresult == 1)
					gfx.mapgo.at(i)->b_modelview = true;
				else
					gfx.mapgo.at(i)->b_modelview = false;
			}

			//chase car collision check
			if (cochasecar->collisionuse)
			{
				DirectX::ContainmentType coresult = cocamera->frustum.Contains(cochasecar->obb);
				if (coresult == 2 || coresult == 1)
				{
					gfx.car.warninguiflag = true;
					if (gfx.cac->hasbeenfound == false)
					{
						gfx.cac->hasbeenfound = true;
						gfx.car.StartDissolveAnimaion();
						gfx.m_Sound->StopSound();
						gfx.m_Sound->PlayIndexSound(Sound::SOUND_LABEL_BGM_chase);
					}
				}
				else
				{
					gfx.car.warninguiflag = false;
				}

				//catch car
				coresult = cocar->obb.Contains(cochasecar->obb);
				if (coresult == 2 || coresult == 1) {
					
					gfx.gamepause = true;
					gfx.physxbase.gStepflag = false;

					gfx.gamescore += 500;
					//reset catch car animaiton status
					gfx.car.catchcount++;
					gfx.catchcar_animationsequenceflag = true;
					gfx.cac->possign_flag = false;
					//gfx.car.warninguiflag = false;
					gfx.catchcar_animaitonindex = 0;
					gfx.gamepausetimer.Restart();
					gfx.Camera3D.DeepCopyTo(&tempcam);
				}
			}

			//npc collision
			for (size_t i = 0; i < gfx.npc.size(); i++)
			{
				auto npc = gfx.npc.at(i);
				if (npc->npcstate <= 1)
				{
					DirectX::ContainmentType coresult = cocar->obb.Contains(npc->starttrigger.obb);
					if (npc->npcstate == 0 && (coresult == 2 || coresult == 1)
						&& std::fabs(vel) <= 3 && !gfx.car.haspassenger && npc->starttrigger.collisionuse)
					{
						//hit girl
						//change npcstate
						npc->starttrigger.collisionuse = false;
						gfx.Camera3D.cameratype = 3;
						npc->npcstate = 1;
						npc->npctimer.Restart();
						npc->uiflag = true;
						gfx.car.haspassenger = true;
						gfx.currentnpcindex = npc->npcindex;
						gfx.m_Sound->PlayIndexSound(Sound::SOUND_LABEL_SE_kyaa);
					}
				}
				if (npc->npcstate >= 2)
				{
					if (npc->endtrigger.collisionuse)
					{
						DirectX::ContainmentType coresult = cocar->obb.Contains(npc->endtrigger.obb);
						if ((coresult == 2 || coresult == 1) && std::fabs(vel) <= 3 && gfx.car.haspassenger)
						{
							//hit destination point
							//change npcstate
							npc->npcstate = 3;
							npc->npctimer.Restart();
							gfx.moneyui.Hide();
							gfx.car.haspassenger = false;
							npc->endtrigger.collisionuse = false;

							//
							gfx.gamescore += gfx.car.cardistance * 5;
							gfx.car.cardistance = 0;
						}
					}
				}
			}

			//count down end
			if (gfx.cac->countdown <= 0)
			{
				gfx.cac->CarEscape();
				gfx.car.StartDissolveAnimaion();
				gfx.m_Sound->StopSound();
				gfx.m_Sound->PlayIndexSound(Sound::SOUND_LABEL_BGM_taxi);
				if (gfx.cac->currentcarindex == 2)
				{
					gfx.Fade(GameState::score);
					return;
				}
			}
		}
		else //game pause
		{
			
			//test code should be delete
			while (!keyboard.KeyBufferIsEmpty())
			{
				KeyboardEvent kbe = keyboard.ReadKey();
				unsigned char keycode = kbe.GetKeyCode();
				if (kbe.IsPress())
				{
					if (keycode == 'L')
					{
						gfx.gamepause = !gfx.gamepause;
					}
				}
			}

			while (!mouse.EventBufferIsEmpty())
			{
				MouseEvent me = mouse.ReadEvent();

				if (mouse.IsRightDown())
				{
					if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
					{
						this->gfx.Camera3D.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0);
					}
				}
			}

			float Camera3DSpeed = 2.0f;
			if (keyboard.KeyIsPressed('W'))
			{
				this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetForwardVector() * Camera3DSpeed * dt);
			}
			if (keyboard.KeyIsPressed('S'))
			{
				this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
			}
			if (keyboard.KeyIsPressed('A'))
			{
				this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetLeftVector() * Camera3DSpeed * dt);
			}
			if (keyboard.KeyIsPressed('D'))
			{
				this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetRightVector() * Camera3DSpeed * dt);
			}


			//catch car animation
			//change camera to 3 position and stay 1 second
			//make some noise
			if (gfx.catchcar_animationsequenceflag) 
			{
				auto pausetime = gfx.gamepausetimer.GetMilisecondsElapsed();
				gfx.b_siflag = true;
				if (pausetime >= 1000){
					//voice
					//change camera
					gfx.catchcar_animaitonindex++;
					gfx.gamepausetimer.Restart();
					
					if (gfx.catchcar_animaitonindex >= gfx.catchcar_animaitonmax)
					{
						//fadeback to the screen
						gfx.gamepause = false;
						gfx.catchcar_animationsequenceflag = false;
						gfx.catchcar_animaitonindex = 0;
						gfx.physxbase.gStepflag = true;
						//caught setting
						
						gfx.cac->canmove = false;
						gfx.cac->CarEscape();

						gfx.gamepausetimer.Stop();
						gfx.b_siflag = false;
						gfx.b_thiflag = false;
						gfx.Fade(GameState::game);

						if (gfx.cac->currentcarindex == 2)
						{
							gfx.cac->currentcarindex = 0;
							gfx.Fade(GameState::score);
							return;
						}
						//tempcam.DeepCopyTo(&gfx.Camera3D);
					}
				}

				if (gfx.catchcar_animaitonindex == 0)
				{
					gfx.m_Sound->PlayIndexSound(Sound::SOUND_LABEL_SE_syoutotu);
					//gfx.m_Sound->PlaySE(Sound::SOUND_LABEL_SE_syoutotu);
					auto testpos = gfx.car.carrender.GetPositionVector() + gfx.car.carrender.GetForwardVector() * 30;
					DirectX::XMFLOAT3 temp;
					DirectX::XMStoreFloat3(&temp, testpos);
					temp = DirectX::XMFLOAT3(temp.x, temp.y + 5, temp.z);
					gfx.Camera3D.SetPosition(temp);
					gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
				}
				else if (gfx.catchcar_animaitonindex == 1)
				{
					gfx.m_Sound->PlayIndexSound(Sound::SOUND_LABEL_SE_syoutotu);
					//gfx.m_Sound->PlaySE(Sound::SOUND_LABEL_SE_syoutotu);
					auto testpos = gfx.car.carrender.GetPositionVector() + gfx.car.carrender.GetLeftVector() * 30;
					DirectX::XMFLOAT3 temp;
					DirectX::XMStoreFloat3(&temp, testpos);
					temp = DirectX::XMFLOAT3(temp.x, temp.y + 3, temp.z);
					gfx.Camera3D.SetPosition(temp);
					gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
				}
				else if (gfx.catchcar_animaitonindex == 2)
				{
					gfx.m_Sound->PlayIndexSound(Sound::SOUND_LABEL_SE_syoutotu);
					//gfx.m_Sound->PlaySE(Sound::SOUND_LABEL_SE_syoutotu);
					gfx.b_thiflag = true;
					auto testpos = gfx.car.carrender.GetPositionVector() + gfx.car.carrender.GetRightVector() * 20;
					DirectX::XMFLOAT3 temp;
					DirectX::XMStoreFloat3(&temp, testpos);
					temp = DirectX::XMFLOAT3(temp.x, temp.y + 2.5, temp.z);
					gfx.Camera3D.SetPosition(temp);
					gfx.Camera3D.SetLookAtPos(gfx.car.carrender.GetPositionFloat3());
				}
			}
		}
		
	}
#pragma endregion
#pragma region GameState::editor
	//if (gfx.gs == GameState::editor) 
//{
//	// キーボードの読み込む
//	while (!keyboard.CharBufferIsEmpty())
//	{
//		unsigned char ch = keyboard.ReadChar();
//	}

//	while (!keyboard.KeyBufferIsEmpty())
//	{
//		KeyboardEvent kbe = keyboard.ReadKey();
//		unsigned char keycode = kbe.GetKeyCode();
//		if (kbe.IsPress())
//		{
//			if (keycode == 'O')
//			{
//				gfx.showImgui = !gfx.showImgui;
//			}

//			if (keycode == 'I')
//			{
//				for (size_t i = 0; i < gfx.mapgo.size(); i++)
//				{
//					gfx.mapgo.at(i)->SetCollisionBoxView(true);
//				}
//			}

//			if (keycode == 'P')
//			{
//				for (size_t i = 0; i < gfx.mapgo.size(); i++)
//				{
//					gfx.mapgo.at(i)->b_use = !gfx.mapgo.at(i)->b_use;
//				}
//			}

//			if (keycode == '\r')
//			{
//				//gfx.Fade(GameState::title);
//			}
//		}

//		if (kbe.IsRelease())
//		{

//		}

//	}

//	//マウスの読み込む
//	while (!mouse.EventBufferIsEmpty())
//	{
//		MouseEvent me = mouse.ReadEvent();

//		if (me.GetType() == MouseEvent::EventType::MPress)
//		{
//			//raycast
//			//gfx.EditorRayCast(mousepos);

//			unsigned int minindx = -1;
//			XMVECTOR mindis = XMVectorSet(999,999,999,999);
//			std::vector<RenderableGameObject*> hitobject;
//			std::vector<unsigned int> indexes;

//			Ray ray = Ray::ScreenToRay(gfx.Camera3D, (float)me.GetPos().x, (float)me.GetPos().y);

//			for (size_t i = 0; i < gfx.mapgo.size(); i++)
//			{
//				if (ray.Hit(gfx.mapgo.at(i)->GetCollisionObject()->obb))
//				{
//					hitobject.push_back(gfx.mapgo.at(i));
//					indexes.push_back(i);
//				}
//			}
//			
//			for (size_t i = 0; i < hitobject.size(); i++)
//			{
//				auto dis = hitobject.at(i)->GetPositionVector() - gfx.Camera3D.GetPositionVector();
//				dis = XMVector3Length(dis);
//				if (XMVectorGetX(dis) <= XMVectorGetX(mindis))
//				{
//					mindis = dis;
//					minindx = hitobject.size() - 1;
//				}
//			}
//			if (minindx != -1)
//			{
//				gfx.m_editor.selectedGo = hitobject.at(minindx);
//				gfx.m_editor.mapgoindex = indexes.at(minindx);
//			}
//				
//		}

//		if (mouse.IsRightDown())
//		{

//			if (me.GetType() == MouseEvent::EventType::RAW_MOVE)
//			{
//				this->gfx.Camera3D.AdjustRotation((float)me.GetPosY() * 0.005f, (float)me.GetPosX() * 0.005f, 0);
//			}
//		}
//	}

//	float Camera3DSpeed = 0.06f;

//	if (keyboard.KeyIsPressed('W'))
//	{
//		this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetForwardVector() * Camera3DSpeed * dt);
//	}
//	if (keyboard.KeyIsPressed('S'))
//	{
//		this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetBackwardVector() * Camera3DSpeed * dt);
//	}
//	if (keyboard.KeyIsPressed('A'))
//	{
//		this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetLeftVector() * Camera3DSpeed * dt);
//	}
//	if (keyboard.KeyIsPressed('D'))
//	{
//		this->gfx.Camera3D.AdjustPosition(this->gfx.Camera3D.GetRightVector() * Camera3DSpeed * dt);
//	}
//	if (keyboard.KeyIsPressed(VK_SPACE))
//	{
//		this->gfx.Camera3D.AdjustPosition(0.0f, Camera3DSpeed * dt, 0.0f);
//	}

//	for (size_t i = 0; i < gfx.mapgo.size(); i++)
//	{
//		gfx.mapgo.at(i)->Update(1.0f,gfx.Camera3D.GetViewMatrix() * gfx.Camera3D.GetProjectionMatrix());
//	}
//}
#pragma endregion

	//fade
	{
		auto fadestate = gfx.fade.fadestate;
		auto rate = gfx.fade.rate;
		if (fadestate == FadeState::Fade_In)
		{
			auto alpha = gfx.fade.fadesprite.color.w - rate;
			alpha = std::clamp(alpha, 0.0f, 1.0f);
			gfx.fade.fadesprite.color = XMFLOAT4(gfx.fade.fadesprite.color.x,
				gfx.fade.fadesprite.color.y, gfx.fade.fadesprite.color.z, alpha);
			alpha <= 0.0f ? fadestate = FadeState::Fade_none : FadeState::Fade_In;
		}
		if (fadestate == FadeState::Fade_Out)
		{
			auto alpha = gfx.fade.fadesprite.color.w + rate;
			alpha = std::clamp(alpha, 0.0f, 1.0f);
			gfx.fade.fadesprite.color = XMFLOAT4(gfx.fade.fadesprite.color.x,
				gfx.fade.fadesprite.color.y, gfx.fade.fadesprite.color.z, alpha);
			if (alpha >= 1.0f)
			{
				gfx.fade.fadestate = FadeState::Fade_In;

				//change game state
				gfx.gs = gfx.tempgs;
				ChangeStats(gfx.gs);
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void Engine::RenderFrame()
{
	this->gfx.RenderFrame();
}

//=============================================================================
// ゲーム状態チェンジする
//=============================================================================
void Engine::ChangeStats(GameState state) 
{
	switch (state)
	{
	case GameState::title:
		//reset title
		gfx.ResetTitle();
		break;
	case GameState::game:
		gfx.ResetGame();
		break;
	case GameState::score:
		gfx.stage.b_use = false;
		gfx.titlestage.b_use = false;
		gfx.ResetScore();
		break;
	case GameState::tutorial:
		//reset tutorial
		gfx.tutorialtexno = 0;
		gfx.stage.b_use = false;
		gfx.titlestage.b_use = false;
		break;
	case GameState::catchcar:
		//catch car fade
		gfx.stage.b_use = false;
		gfx.titlestage.b_use = false;
		break;
	}
}
