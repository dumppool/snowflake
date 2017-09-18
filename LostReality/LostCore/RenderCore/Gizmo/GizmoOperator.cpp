#include "stdafx.h"

LostCore::FGizmoOperator::FGizmoOperator() 
	: Target(nullptr)
	, BaseOffset(INFINITY)
	, ActivedComponent()
{
	World.SetIdentity();
	Components.clear();
}

LostCore::FGizmoOperator::~FGizmoOperator()
{
	Fini();
}

bool LostCore::FGizmoOperator::Load(const string & url)
{
	FJson config;
	if (FDirectoryHelper::Get()->GetGizmoJson(url, config))
	{
		return Config(config);
	}

	return false;
}

bool LostCore::FGizmoOperator::Config(const FJson & config)
{
	if (config.find(K_PLACER) != config.end())
	{
		FJson modelConfig;
		if (FDirectoryHelper::Get()->GetModelJson(config[K_PLACER].get<string>(), modelConfig))
		{
			auto model = FModelFactory::NewModel(modelConfig);
			if (model != nullptr)
			{
				FComp comp;
				FFloat4x4 w;

				// PlacerX
				comp.Id = EOp::PlacerX;
				comp.Model = model;
				comp.Model->EnableDepthTest(false);
				comp.Model->SetColor(FColor128((uint32)0xffff0000));
				comp.Local = w.SetRotate(FFloat3(0.0f, 90.0f, 0.0f));
				comp.Plane = FPlane(FFloat3(0.0f, 1.0f, 0.0f), 0.0f);
				Components.push_back(comp);

				// PlacerY
				comp.Id = EOp::PlacerY;
				comp.Model = FModelFactory::NewModel(modelConfig);
				comp.Model->EnableDepthTest(false);
				comp.Model->SetColor(FColor128((uint32)0xff00ff00));
				comp.Local = w.SetRotate(FFloat3(90.0f, 0.0f, 0.0f));
				comp.Plane = FPlane(FFloat3(0.0f, 0.0f, 1.0f), 0.0f);
				Components.push_back(comp);

				// PlacerZ
				comp.Id = EOp::PlacerZ;
				comp.Model = FModelFactory::NewModel(modelConfig);
				comp.Model->EnableDepthTest(false);
				comp.Model->SetColor(FColor128((uint32)0xff0000ff));
				comp.Local = w.SetRotate(FFloat3(0.0f, 0.0f, 0.0f));
				comp.Plane = FPlane(FFloat3(1.0f, 0.0f, 0.0f), 0.0f);
				Components.push_back(comp);

				// TODO: Rotator...

				return true;
			}
		}
	}

	return false;
}

bool LostCore::FGizmoOperator::RayTest(FRay & ray, bool active)
{
	float minDist = ray.Distance;
	ActivedComponent.Reset();

	auto travel = [&](FComp& comp)
	{
		auto m = comp.Model;
		if (m == nullptr || m->GetBoundingBox() == nullptr)
		{
			return;
		}

		m->GetBoundingBox()->bVisible = false;
		if (RayBoxIntersect(ray, *m->GetBoundingBox(), m->GetWorldMatrix().Invert(), minDist))
		{
			ActivedComponent = comp;
			ray.Distance = minDist;
		}
	};

	for_each(Components.begin(), Components.end(), travel);
	if (ActivedComponent.Id != EOp::Null)
	{
		if (active)
		{
			// TODO: Need a highlight method.
			ActivedComponent.Model->GetBoundingBox()->bVisible = true;
		}
		else
		{
			ActivedComponent.Reset();
		}

		return true;
	}

	return false;
}

void LostCore::FGizmoOperator::MoveGizmo(const FRay & ray)
{
	if (ActivedComponent.Id == EOp::Null || Target == nullptr)
	{
		return;
	}

	auto compIt = find(Components.begin(), Components.end(), ActivedComponent);
	if (compIt == Components.end())
	{
		return;
	}

	auto world = compIt->Model->GetWorldMatrix();
	float dist;
	if (RayPlaneIntersect(ray, compIt->Plane.GetTransformed(world), dist, false))
	{
		auto vec = ray.GetPoint(dist) - world.GetOrigin();
		float offset;
		if (compIt->Id <= EOp::PlacerZ && EOp::PlacerX <= compIt->Id)
		{
			offset = vec.Dot(world.GetForwardVector().GetNormal());

			if (BaseOffset == INFINITY)
			{
				BaseOffset = offset;
			}
			else
			{
				auto dOffset = offset - BaseOffset;
				auto targetWorld = Target->GetWorldMatrix();
				targetWorld.AddTranslate(world.GetForwardVector().GetNormal() * dOffset);
				Target->SetWorldMatrix(targetWorld);
			}
		}
		else if (compIt->Id <= EOp::RotatorZ && EOp::RotatorX <= compIt->Id)
		{
		}
	}
}

void LostCore::FGizmoOperator::StopMove()
{
	BaseOffset = INFINITY;
}

void LostCore::FGizmoOperator::Tick()
{
	if (Target == nullptr)
	{
		World.SetIdentity();
		return;
	}

	World = Target->GetWorldMatrix();
	for (auto& comp : Components)
	{
		if (comp.Model != nullptr)
		{
			auto world = comp.Local * World;
			comp.Model->SetWorldMatrix(world);
			comp.Model->Tick();
		}
	}
}

void LostCore::FGizmoOperator::Draw()
{
	if (Target == nullptr)
	{
		return;
	}

	for (auto& comp : Components)
	{
		if (comp.Model != nullptr)
		{
			comp.Model->Draw();
		}
	}
}

void LostCore::FGizmoOperator::SetTarget(FBasicModel * target)
{
	Target = target;
}

LostCore::FBasicModel * LostCore::FGizmoOperator::GetTarget()
{
	return Target;
}

void LostCore::FGizmoOperator::Fini()
{
	for (auto& comp : Components)
	{
		SAFE_DELETE(comp.Model);
	}

	Components.clear();
	Target = nullptr;
}

LostCore::FGizmoOperator::FComp::FComp()
	: Model(nullptr)
{
	Reset();
}

void LostCore::FGizmoOperator::FComp::Reset()
{
	Id = EOp::Null;
	if (Model != nullptr)
	{
		Model->GetBoundingBox()->bVisible = false;
		Model = nullptr;
	}

	Local.SetIdentity();
}

bool LostCore::FGizmoOperator::FComp::operator==(const FComp& rhs) const
{
	return Id == rhs.Id;
}
