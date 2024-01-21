// Jolt Physics Library (https://github.com/jrouwe/JoltPhysics)
// SPDX-FileCopyrightText: 2021 Jorrit Rouwe
// SPDX-License-Identifier: MIT

#include <TestFramework.h>

#include <Tests/Shapes/BoxShapeTest.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Common/Game/PhysicsManager.h>

JPH_IMPLEMENT_RTTI_VIRTUAL(BoxShapeTest)
{
	JPH_ADD_BASE_CLASS(BoxShapeTest, Test)
}

void BoxShapeTest::Initialize()
{
	// Floor
	//CreateFloor();

	// Different sized boxes
	//Body &body1 = *mBodyInterface->CreateBody(BodyCreationSettings(new BoxShape(Vec3(20, 1, 1)), RVec3(0, 10, 0), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING));
	//mBodyInterface->AddBody(body1.GetID(), EActivation::Activate);

	//Body &body2 = *mBodyInterface->CreateBody(BodyCreationSettings(new BoxShape(Vec3(2, 3, 4)), RVec3(0, 10, 10), Quat::sRotation(Vec3::sAxisZ(), 0.25f * JPH_PI), EMotionType::Dynamic, Layers::MOVING));
	//mBodyInterface->AddBody(body2.GetID(), EActivation::Activate);

	//Body &body3 = *mBodyInterface->CreateBody(BodyCreationSettings(new BoxShape(Vec3(0.5f, 0.75f, 1.0f)), RVec3(0, 10, 20), Quat::sRotation(Vec3::sAxisX(), 0.25f * JPH_PI) * Quat::sRotation(Vec3::sAxisZ(), 0.25f * JPH_PI), EMotionType::Dynamic, Layers::MOVING));
	//mBodyInterface->AddBody(body3.GetID(), EActivation::Activate);

	//BodyInterface& body_interface = *mBodyInterface;
	//const float wall_width = 10;
	//const float wall_height = 50;
	//const float m_cxClient = 704;
	//const float m_cyClient = 448;
	//const float GoalWidth = 100;
	//const float goal_post_width = 5.0f;
	//const float goal_wall_width = (m_cyClient / 2 - GoalWidth / 2) / 2 + goal_post_width;
	//const float out_wall_width = m_cxClient / 2 + goal_post_width;
	//const float Top = 20;
	//const float Left = 20;
	//const float Right = 684;
	//const float Bottom = 428;
	//const float m_pRedGoalRightPost = 274;
	//const float m_pBlueGoalRightPost = 274;

	//BodyCreationSettings settings;
	//settings.SetShape(new SphereShape(1.0f));
	//settings.mPosition = RVec3(m_cxClient/2, 20, m_cyClient/2);// RVec3(0, 0.5f, 0);
	//settings.mMotionType = EMotionType::Dynamic;
	////settings.mMotionQuality = EMotionQuality::LinearCast;
	////settings.mLinearVelocity = Vec3(-24, 0, -12);
	//settings.mFriction = 0.0f;
	//settings.mRestitution = 0.6f;
	//settings.mObjectLayer = Layers::MOVING;
	//Body* mBody = mBodyInterface->CreateBody(settings);
	//mBodyInterface->AddBody(mBody->GetID(), EActivation::Activate);

	//// Create Floor
	//BoxShapeSettings floor_shape_settings(Vec3(m_cxClient / 2, 1.0f, m_cyClient / 2));
	//ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	//ShapeRefC floor_shape = floor_shape_result.Get();
	//BodyCreationSettings floor_settings(floor_shape, RVec3(m_cxClient / 2, -1.0_r, m_cyClient / 2), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	////floor_settings.mFriction = -0.000001;
	//Body* floor = body_interface.CreateBody(floor_settings);
	//body_interface.AddBody(floor->GetID(), EActivation::DontActivate);


	//{
	//	BoxShapeSettings goal_wall_shape_settings(Vec3(wall_width, wall_height, goal_wall_width));
	//	ShapeSettings::ShapeResult goal_wall_shape_result = goal_wall_shape_settings.Create();
	//	ShapeRefC goal_wall_shape = goal_wall_shape_result.Get();
	//	BodyCreationSettings goal_wall_settings(goal_wall_shape, RVec3(Left - wall_width, wall_height - 10, Top + goal_wall_width - goal_post_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	//	Body* goal_wall = body_interface.CreateBody(goal_wall_settings);
	//	body_interface.AddBody(goal_wall->GetID(), EActivation::DontActivate);
	//}

	//{
	//	BoxShapeSettings goal_wall_shape_settings(Vec3(wall_width, wall_height, goal_wall_width));
	//	ShapeSettings::ShapeResult goal_wall_shape_result = goal_wall_shape_settings.Create();
	//	ShapeRefC goal_wall_shape = goal_wall_shape_result.Get();
	//	BodyCreationSettings goal_wall_settings(goal_wall_shape, RVec3(Left - wall_width, wall_height - 10, m_pRedGoalRightPost + goal_wall_width + goal_post_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	//	Body* goal_wall = body_interface.CreateBody(goal_wall_settings);
	//	body_interface.AddBody(goal_wall->GetID(), EActivation::DontActivate);
	//}

	//{
	//	BoxShapeSettings goal_wall_shape_settings(Vec3(wall_width, wall_height, goal_wall_width));
	//	ShapeSettings::ShapeResult goal_wall_shape_result = goal_wall_shape_settings.Create();
	//	ShapeRefC goal_wall_shape = goal_wall_shape_result.Get();
	//	BodyCreationSettings goal_wall_settings(goal_wall_shape, RVec3(Right + wall_width, wall_height - 10, Top + goal_wall_width - goal_post_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	//	Body* goal_wall = body_interface.CreateBody(goal_wall_settings);
	//	body_interface.AddBody(goal_wall->GetID(), EActivation::DontActivate);
	//}

	//{
	//	BoxShapeSettings goal_wall_shape_settings(Vec3(wall_width, wall_height, goal_wall_width));
	//	ShapeSettings::ShapeResult goal_wall_shape_result = goal_wall_shape_settings.Create();
	//	ShapeRefC goal_wall_shape = goal_wall_shape_result.Get();
	//	BodyCreationSettings goal_wall_settings(goal_wall_shape, RVec3(Right + wall_width, wall_height - 10, m_pBlueGoalRightPost + goal_wall_width + goal_post_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	//	Body* goal_wall = body_interface.CreateBody(goal_wall_settings);
	//	body_interface.AddBody(goal_wall->GetID(), EActivation::DontActivate);
	//}

	//{
	//	BoxShapeSettings line_wall_shape_settings(Vec3(out_wall_width, wall_height, wall_width));
	//	ShapeSettings::ShapeResult line_wall_shape_result = line_wall_shape_settings.Create();
	//	ShapeRefC line_wall_shape = line_wall_shape_result.Get();
	//	BodyCreationSettings line_wall_settings(line_wall_shape, RVec3(out_wall_width, wall_height - 10, Top - wall_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	//	Body* line_wall = body_interface.CreateBody(line_wall_settings);
	//	body_interface.AddBody(line_wall->GetID(), EActivation::DontActivate);
	//}

	//{
	//	BoxShapeSettings line_wall_shape_settings(Vec3(out_wall_width, wall_height, wall_width));
	//	ShapeSettings::ShapeResult line_wall_shape_result = line_wall_shape_settings.Create();
	//	ShapeRefC line_wall_shape = line_wall_shape_result.Get();
	//	BodyCreationSettings line_wall_settings(line_wall_shape, RVec3(out_wall_width, wall_height - 10, Bottom + wall_width), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	//	Body* line_wall = body_interface.CreateBody(line_wall_settings);
	//	body_interface.AddBody(line_wall->GetID(), EActivation::DontActivate);
	//}

}
